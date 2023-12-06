#ifndef __CONFIG__HPP__MONITOR
#define __CONFIG__HPP__MONITOR

#include <algorithm>
#include <string>
#include <memory>
#include <mutex>
#include <iostream>
#include <list>
#include <vector>
#include <sstream>
#include <exception>
#include <stdexcept>
#include <shared_mutex>
#include <functional>
#include <boost/lexical_cast.hpp>
#include <yaml-cpp/yaml.h>

namespace monitor {

#define SPTR_DEFINE(type) \
    using sptr = std::shared_ptr<type>;

#define READ_LOCK(m) \
    std::shared_lock<std::shared_timed_mutex> lock((m));

#define WRITE_LOCK(m) \
    std::unique_lock<std::shared_timed_mutex> lock((m));

template <class From, class To>
class LexicalCast{
public:
    To operator() (const From &from) {
        return boost::lexical_cast<To>(from);
    }
};

//偏特化 ： string -> vector<T>
template<class T>
class LexicalCast<std::string, std::vector<T>>  {
public:
    std::vector<T> operator() (const std::string &from) {
        YAML::Node node = YAML::Load(from);
        std::vector<T> result;
        if(node.IsSequence()){
            std::stringstream ss;
            for(const auto &it : node){
                ss.str("");
                ss << it;
                result.emplace_back(
                    LexicalCast<std::string, T>()(ss.str())
                );
            }
        }else{

        }
        return result;
    }
};

//将数据转换为，YAML的序列
//偏特化 ：vector<T> -> string
// {1，2， 3} -> 
    // -1,
    // -2,
    // -3
template<class T>
class LexicalCast<std::vector<T>, std::string> {
public:
    std::string operator() (const std::vector<T> &from) {
        YAML::Node node;
        for(const auto &it : from) {
            node.push_back(
                YAML::Load(LexicalCast<T, std::string>()(it))
            );
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<class T>
class LexicalCast<std::string, std::list<T>> {
public:
    std::list<T> operator() (const std::string &from) {
        YAML::Node node = YAML::Load(from);
        std::list<T> result;
        if(node.IsSequence()){
            std::stringstream ss;
            for(const auto &it : node) {
                ss.str("");
                ss << it;
                result.push_back(
                    LexicalCast<std::string, T>()(ss.str())
                );
            }
        }else{}
        return result;
    }
};

//将数据转换为，YAML的序列
//偏特化 ：list<T> -> string
template<class T>
class LexicalCast<std::list<T>, std::string>{
public:
    std::string operator() (const std::list<T> &from) {
        YAML::Node node;
        for(const auto &it : from) {
            node.push_back(
                YAML::Load(LexicalCast<T, std::string>()(it))
            );
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};




// 偏特化， std::map<std::string, T> -> std::string ，
//
template <class T>
class LexicalCast<std::map<std::string, T>, std::string> {
public:
    std::string operator()(const std::map<std::string, T> &from) {
        YAML::Node node;
        for(const auto &[f_, s_] : from){
            node[f_] = YAML::Load(
                LexicalCast<T, std::string>()(s_)
            );
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template <class T>
class LexicalCast<std::string, std::map<std::string, T>> {
public:
    std::map<std::string, T> operator()(const std::string &from) {
        YAML::Node node = YAML::Load(from);
        std::map<std::string, T> result;
        if(node.IsMap()){
            std::stringstream ss;
            for(const auto &it : node){
                ss.str("");
                ss << it.second;
                result.emplace(
                    it.first.as<std::string>(),
                    LexicalCast<std::string, T>()(ss.str())
                );
            }
        }else{}

        return result;
    }
};

template <class T>
class LexicalCast<std::string, std::set<T>> {
public:
    std::set<T> operator() (const std::string &from) {
        YAML::Node node = YAML::Load(from);
        std::set<T> result;
        if(node.IsSequence()){
            std::stringstream ss;
            for(const auto &it : node) {
                ss.str("");
                ss << it;
                result.insert(
                    LexicalCast<std::string, T>()(ss.str())
                );
            }
        }else {}
        return result;
    }
};

template <class T>
class LexicalCast<std::set<T>, std::string> {
public:
    std::string operator() (const std::set<T> &from) {
        YAML::Node node;
        for(const auto & it : from ) {
            node.push_back(
                LexicalCast<T, std::string>()(it)
            );
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

class ConfigVarBase {
public:
    SPTR_DEFINE(ConfigVarBase);
protected:
    // 配置项的名称
    std::string name_;
    std::string decsription_;

public:
    explicit ConfigVarBase (const std::string &name, const std::string &des) :
            name_(name), decsription_(des) {
        std::transform(begin(name_), end(name_), begin(name_), ::tolower);
    }
    virtual ~ConfigVarBase () = default;
    const std::string& getName() const {
        return name_;
    }
    const std::string& getDescription() const {
        return decsription_;
    }
    virtual std::string toString() const = 0;
    virtual bool formString(const std::string &val) = 0;
};


/** ConfigVar Class
 * 通用型配置项的模板类
 * 模板参数:
 *      T               配置项的值的类型
 *      ToStringFN      {functor<std::string(T&)>} 将配置项的值转换为 std::string
 *      FromStringFN    {functor<T(const std::string&)>} 将 std::string 转换为配置项的值
 * */
template<
    class T,
    class ToStringFun = LexicalCast<T, std::string>,
    class FromStringFun = LexicalCast<std::string, T>>
class ConfigVar : public ConfigVarBase{
public:
    SPTR_DEFINE(ConfigVar);
    using CallBackFun = std::function<void(const T& old_, const T&new_)>;

private:
    T value_;
    std::map<uint64_t, CallBackFun> callbacks_;
    mutable std::shared_timed_mutex mutex_;

public:
    explicit ConfigVar(const std::string &name, const T &value, const std::string &des) :
        ConfigVarBase(name, des), value_(value) {}

    T getValue() const {
        READ_LOCK(mutex_);
        return value_;
    }
    void setValue(const T &value) {
        {
            READ_LOCK(mutex_);
            if(value == value_){
                return;
            }
            for(const auto &[_, fun_] : callbacks_ ) {
                fun_(value_, value);
            }
        }
        WRITE_LOCK(mutex_);
        value_ = value;
    }
    std::string toString() const override {
        try{
            return ToStringFun()(getValue());
        }catch(std::exception &e){
            std::cerr << "ConfigVar::toString exception "
                     << e.what()
                     << std::endl;
        }
        return {"[Error !]"};
    }
    bool formString(const std::string &from) override {
        try{
            setValue(FromStringFun()(from));
            return true;
        }catch(std::exception &e){
            std::cerr << "ConfigVar::fromString exception "
                     << e.what()
                     << std::endl;
        }
        return false;
    }
    uint64_t addListener(CallBackFun cb) {
        static uint64_t cbid = 0;
        WRITE_LOCK(mutex_);
        callbacks_[cbid] = cb;
        return cbid++;
    }
    void delListener(const uint64_t &cbid){
        WRITE_LOCK(mutex_);
        callbacks_.erase(cbid);
    }
    CallBackFun getListener(const uint64_t &cbid) const {
        READ_LOCK(mutex_);
        auto it = callbacks_.find(cbid);
        if(it == callbacks_.end()) {
            return nullptr;
        }
        return it->second;
    }
    void clearListen() {
        WRITE_LOCK(mutex_);
        callbacks_.clear();
    }
};

class Config {
public:
    using ConfigValueMap = std::map<std::string, ConfigVarBase::sptr>;
private:
    static ConfigValueMap& GetData() {
        static ConfigValueMap data_;
        return data_;
    }
    static std::shared_timed_mutex&  GetSharedMutex()  {
        static std::shared_timed_mutex mutex_;
        return mutex_;
    }


public:
    static ConfigVarBase::sptr 
    Lookup(const std::string &name) {
        READ_LOCK(GetSharedMutex());
        auto it = GetData().find(name);
        if(it == GetData().end()){
            return nullptr;
        }
        return it->second;
    }

    template <class T>
    static typename ConfigVar<T>::sptr
    Lookup(const std::string &name) {
        auto vbase = Lookup(name);
        if(vbase == nullptr) {
            return nullptr;
        }
        auto res = std::dynamic_pointer_cast<ConfigVar<T>>(vbase);
        if(res == nullptr){

             std::cerr << "Config::Lookup<T> exception, 无法转换 ConfigVar<T> "
                           "的实际类型到模板参数类型 T" << std::endl;
            throw std::bad_cast();
        }
        return res;
    }

    template <class T>
    static typename ConfigVar<T>::sptr 
    Lookup
    (const std::string &name, const T &value, const std::string &des={}) {
        auto tmp = Lookup<T>(name);
        if(tmp != nullptr) {
            return tmp;
        }
        if(name.end() != 
            std::find_if(name.begin(), name.end(), [](const char c){
                return !(std::isalnum(c) || c=='.' || c=='_');
            })) {
            std::cerr << 
                "Congif::Lookup exception, 参数只能是字母数字点或下划线" << std::endl;
            throw std::invalid_argument(name);
        }

        auto res = std::make_shared<ConfigVar<T>>(name, value, des);
        WRITE_LOCK(GetSharedMutex());
        GetData()[name] = res;
        return res;
    }

    static void LoadFromYAML(const YAML::Node &root) {
        std::vector<std::pair<std::string, YAML::Node>> nodes_;
        TraversalNode(root, "", nodes_);

        for (const auto &[key_, node] : nodes_) {
            std::string name = key_;
            if(name.empty()){
                continue;
            }
            std::transform(name.begin(), name.end(), name.begin(), ::tolower);
            auto vsptr = Lookup(name);
            if(vsptr != nullptr) {
                std::stringstream ss;
                ss << node;
                vsptr->formString(ss.str());
            }
        }
    }

private:
    static void
    TraversalNode(const YAML::Node &node, const std::string &name,
                  std::vector<std::pair<std::string, YAML::Node>> &out){

        auto it = std::find_if(
            out.begin(), 
            out.end(),
            [&name](const auto &item){
                return item.first == name;
            }
        );
        if(out.end() != it) {
            it->second = node;
        }else{
            out.emplace_back(name, node);
        }
        if(node.IsMap()){
            for(auto it=node.begin(); it!=node.end(); ++it){
                TraversalNode(
                    it->second,
                    name.empty()?it->first.Scalar():(name+"."+it->first.Scalar()),
                    out
                );
            }
        }else if(node.IsSequence()){
            for(size_t i=0; i<node.size(); ++i) {
                TraversalNode(
                    node[i],
                    name+"."+std::to_string(i),
                    out
                );
            }
        }
    }
};
std::ostream& operator<<(std::ostream& out, const ConfigVarBase& cvb);
}
#endif