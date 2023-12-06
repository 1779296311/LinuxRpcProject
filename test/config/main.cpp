#include "config_develop.hpp"

auto config_system_port = 
    monitor::Config::Lookup<int>(std::string{"system.port"}, 6666);
auto config_test_list = monitor::Config::Lookup<std::vector<std::string>>(
    "test_list", std::vector<std::string>{});
auto config_test_linklist = monitor::Config::Lookup<std::list<std::string>>(
    "test_linklist", std::list<std::string>{"list", "string"});
auto config_test_map = monitor::Config::Lookup<std::map<std::string, std::string>>(
    "test_map", std::map<std::string, std::string>{
                    std::make_pair("map1", "srting"),
                    std::make_pair("map2", "srting"),
                    std::make_pair("map3", "srting")});
auto config_test_set = monitor::Config::Lookup<std::set<int>>(
    "test_set", std::set<int>{10, 20, 30});



struct Goods
{
    std::string name;
    double price;

    std::string toString() const
    {
        std::stringstream ss;
        ss << "**" << name << "** $" << price;
        return ss.str();
    }

    bool operator==(const Goods& rhs) const
    {
        return name == rhs.name &&
               price == rhs.price;
    }
};

std::ostream& operator<<(std::ostream& out, const Goods& g)
{
    out << g.toString();
    return out;
}

namespace monitor
{
// zjl::LexicalCast 针对自定义类型的偏特化
template <>
class LexicalCast<std::string, Goods>
{
public:
    Goods operator()(const std::string& source)
    {
        auto node = YAML::Load(source);
        Goods g;
        if (node.IsMap())
        {
            g.name = node["name"].as<std::string>();
            g.price = node["price"].as<double>();
        }
        return g;
    }
};

template <>
class LexicalCast<Goods, std::string>
{
public:
    std::string operator()(const Goods& source)
    {
        YAML::Node node;
        node["name"] = source.name;
        node["price"] = source.price;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};
}

auto config_test_user_type = 
    monitor::Config::Lookup<Goods>("user.goods", Goods{});
auto config_test_uset_type_list = 
    monitor::Config::Lookup<std::vector<Goods>>("user.goods_list", std::vector<Goods>{});

// ===============================================

// 测试通过解析 yaml 文件更新配置项
void TEST_loadConfig(const std::string& path)
{
    std::cout <<  "call TEST_loadConfig 测试通过解析 yaml 文件更新配置项" << std::endl;;
    YAML::Node config;
    try
    {
        config = YAML::LoadFile(path);
    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
    monitor::Config::LoadFromYAML(config);
}

// 测试配置项的 toString 方法
void TEST_ConfigVarToString()
{
    std::cout <<  "call TEST_defaultConfig 测试获取默认的配置项" << std::endl;
    std::cout << *config_system_port << std::endl;
    std::cout << *config_test_list << std::endl;
    std::cout << *config_test_linklist << std::endl;
    std::cout << *config_test_map << std::endl;
    std::cout << *config_test_set << std::endl;
    std::cout << *config_test_user_type << std::endl;
    std::cout << *config_test_uset_type_list << std::endl;
}

// 测试获取并使用配置的值
void TEST_GetConfigVarValue()
{
    std::cout <<"call TEST_GetConfigVarValue 测试获取并使用配置的值" << std::endl;
// 遍历线性容器的宏
#define TSEQ(config_var)                                             \
    std::cout << "name = " << config_var->getName() << "; value = "; \
    for (const auto& item : config_var->getValue())                  \
    {                                                                \
        std::cout << item << ", ";                                   \
    }                                                                \
    std::cout << std::endl;

    TSEQ(config_test_list);
    TSEQ(config_test_linklist);
    TSEQ(config_test_set);
    TSEQ(config_test_uset_type_list);
#undef TSEQ
// 遍历映射容器的宏
#define TMAP(config_var)                                                \
    std::cout << "name = " << config_var->getName() << "; value = ";    \
    for (const auto& pair : config_var->getValue())                     \
    {                                                                   \
        std::cout << "<" << pair.first << ", " << pair.second << ">, "; \
    }                                                                   \
    std::cout << std::endl;

    TMAP(config_test_map);
#undef TMAP
}

// 测试获取不存在的配置项
void TEST_nonexistentConfig() {
    std::cout <<  "call TEST_nonexistentConfig 测试获取不存在的配置项\n";
    auto log_name = monitor::Config::Lookup("nonexistent");
    if (!log_name)
    {
        std::cout << "non value" << std::endl;;
    }
}

int main()
{
    config_system_port->addListener(
        [](const int& old_value, const int& new_value) {
            std::cout << 
                "配置项 system.port 的值被修改" << old_value << " -> " <<new_value << '\n';
        });
    TEST_ConfigVarToString();
    TEST_GetConfigVarValue();
    TEST_loadConfig("/work/test/config/test.yaml");
    TEST_ConfigVarToString();
    TEST_GetConfigVarValue();
    TEST_nonexistentConfig();

    YAML::Node node;
    auto str = node["node"] ? node["node"].as<std::string>() : "";
    std::cout << str << std::endl;
    return 0;
}