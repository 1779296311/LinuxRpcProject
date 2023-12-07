#ifndef __LOG_HPP_MONITOR
#define __LOG_HPP_MONITOR


#include <string>
#include <vector>
#include <memory>
#include <map>
#include <fstream>
#include <list>
#include <thread>
#include <mutex>
#include <sstream>
#include "singleton.hpp"
#include "config.hpp"

namespace monitor{

#define LOG_GENERATE(logger, level, message)    \
    logger->log(                                \
        std::make_shared<monitor::LogEvent>(    \
            level,                              \
            __FILE__,                           \
            __LINE__,                           \
            std::this_thread::get_id(),         \
            std::time(nullptr),                 \
            message                             \
        )                                       \
    )

#define LOG_DEBUG(logger, messgae) LOG_GENERATE(logger, LogLevel::Level::DEBUG, message)
#define LOG_INFO(logger, messgae) LOG_GENERATE(logger, LogLevel::Level::INFO, message)
#define LOG_WARN(logger, messgae) LOG_GENERATE(logger, LogLevel::Level::WARN, message)
#define LOG_ERROR(logger, messgae) LOG_GENERATE(logger, LogLevel::Level::ERROR, message)
#define LOG_FATAL(logger, messgae) LOG_GENERATE(logger, LogLevel::Level::FATAL, message)

#define LOG_FMT(logger, level, format, argv...) {               \
    char *buff = nullptr;                                       \
    int len = asprintf(&buff, format, argv);                    \
    if(len != -1){                                              \
        LOG_GENERATE(logger, level, std::string(buff, len));    \
        free(buff);                                             \
    }                                                           \
}

#define LOG_FMT_DEBUG(logger, format, argv...) LOG_FMT(logger, monitor::LogLevel::Level::DEBUG, format, argv)
#define LOG_FMT_INFO(logger, format, argv...) LOG_FMT(logger, monitor::LogLevel::Level::INFO, format, argv)
#define LOG_FMT_WARN(logger, format, argv...) LOG_FMT(logger, monitor::LogLevel::Level::WARN,format, argv)
#define LOG_FMT_ERROR(logger, format, argv...) LOG_FMT(logger, monitor::LogLevel::Level::ERROR, format, argv)
#define LOG_FMT_FATAL(logger, format, argv...) LOG_FMT(logger, monitor::LogLevel::Level::FATAL, format, argv)

#define GET_ROOT_LOGGER() monitor::LoggerManager::GetInstance()->getGlobal();

#define ADD_SPTR_DEFINE(type) \
    using sptr = std::shared_ptr<type>;

class LogLevel{
public:
    enum class Level{
        UNKNOW = 0,
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };
    static std::string levelToString(LogLevel::Level level);
};

class LogAppenderConfig{
public:
    enum class Type{
        STDOUT = 0,
        FILE = 1
    };
    LogAppenderConfig::Type type_;
    LogLevel::Level level_;
    // 日志打印格式
    std::string formatter_;
    // 目标文件路径
    std::string file_;

    LogAppenderConfig(LogAppenderConfig::Type type_, 
                      LogLevel::Level level_, 
                      const std::string &formatter_,
                      const std::string &file_ ) :
                      type_(type_), level_(level_), 
                      formatter_(formatter_), 
                      file_(file_) 
                    {}

public:
    LogAppenderConfig() :
        type_(Type::STDOUT), level_(LogLevel::Level::UNKNOW) {}

    bool operator== (const LogAppenderConfig &other) const {
        return type_ == other.type_ && level_ == other.level_ && 
               formatter_ == other.formatter_ && file_ == other.file_;
    }
};

class LogConfig{
public:
    LogLevel::Level level_;
    std::string name_;
    std::string formatter_;
    std::vector<LogAppenderConfig> appenders_;
    LogConfig(LogLevel::Level level, const std::string &name, const std::string &formatter) :
        level_(level), name_(name), formatter_(formatter) {}
public:
    LogConfig() :
        level_(LogLevel::Level::UNKNOW) {}
    bool operator== (const LogConfig& other) const {
        return name_ == other.name_;
    }
};

template<>
class LexicalCast<std::string, monitor::LogAppenderConfig> {
public:
    LogAppenderConfig operator() (const std::string &from) {
        auto node = YAML::Load(from);
        if(!node.IsMap()){
            return  LogAppenderConfig {};
        }
        return LogAppenderConfig {
            static_cast<LogAppenderConfig::Type> (
                node["type"] ? (node["type"].as<int>()) : 0
            ),
            static_cast<LogLevel::Level>(
                node["level"] ? (node["level"].as<int>()) : 0
            ),
            node["formatter"] ? node["formatter"].as<std::string>() : std::string{},
            node["file"] ? node["file"].as<std::string>() : std::string{}
        };
    }
};

template<>
class LexicalCast<monitor::LogAppenderConfig, std::string>{
public:
    std::string operator() (const monitor::LogAppenderConfig &from) {
        YAML::Node node;
        node["type"] = static_cast<int>(from.type_);
        node["level"] = static_cast<int>(from.level_);
        node["file"] = from.file_;
        node["fformatter"] = from.formatter_;
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};

template<>
class LexicalCast<monitor::LogConfig, std::string>{
public:
    std::string operator() (const monitor::LogConfig &from ) {
        YAML::Node node;
        node["name"] = from.name_;
        node["level"] = static_cast<int>(from.level_);
        node["formatter"] = from.formatter_;
        for(auto &appd : from.appenders_) {
            node["appender"].push_back(
                LexicalCast<LogAppenderConfig, std::string>()(appd)
            );
        }
        std::stringstream ss;
        ss << node;
        return ss.str();
    }
};


template<>
class LexicalCast<std::string, monitor::LogConfig> {
public:
    LogConfig operator() (const std::string &from) {
        auto node = YAML::Load(from);
        if(!node.IsMap()){
            return LogConfig {};
        }
        LogConfig result(
            node["level"] ? 
                (static_cast<LogLevel::Level>(node["level"].as<int>())) : 
                LogLevel::Level::UNKNOW ,
            node["name"] ? 
                (node["name"].as<std::string>()) :
                std::string{},
            node["formatter"] ?
                (node["formatter"].as<std::string>()) :
                std::string{}
        );
        if(node["appender"] && node["appender"].IsSequence()){
            std::stringstream ss;
            for(const auto& appd : node["appender"]) {
                ss.str("");
                ss << appd;
                result.appenders_.emplace_back(
                    LexicalCast<std::string, monitor::LogAppenderConfig>()(ss.str())
                );
                auto it = result.appenders_.end() - 1;
                if(it->formatter_.empty()){
                    it->formatter_ = result.formatter_;
                }
                if(it->level_ == LogLevel::Level::UNKNOW){
                    it->level_ = result.level_;
                }
            }
        }
        return result;
    }
};


//日志信息
class LogEvent{
public:
    ADD_SPTR_DEFINE(LogEvent);
private:
    LogLevel::Level level_;
    std::string file_;
    uint32_t line_{0};
    std::thread::id tid_;
    std::time_t time_;
    std::string content_;

public:
    LogEvent(const LogLevel::Level &level, 
             const std::string &file, 
             const uint32_t &line, 
             const std::thread::id &tid,
             const time_t &time,
             const std::string &content) :
             level_(level), file_(file), line_(line), 
             tid_(tid), time_(time), content_(content)
             { }
    const LogLevel::Level& getLevel() const {
        return level_;
    } 
    const std::string& getFile() const {
        return file_;
    }
    const uint32_t getLine() const {
        return line_;
    }
    const std::thread::id getThreadId() const {
        return tid_;
    }
    const std::time_t& getTime() const {
        return time_;
    }
    const std::string& getContent() const { 
        return content_; 
    }
    void setLevel(const LogLevel::Level &level) {
        level_ = level_;
    }

};

//格式化
class LogFormatter {
public:
    ADD_SPTR_DEFINE(LogFormatter);

    class FormatItem{
    public:
        ADD_SPTR_DEFINE(FormatItem);
        virtual void format(std::ostream& out, LogEvent::sptr ev) = 0;
        virtual ~FormatItem() = default;
    };
private:
    // 日志格式化字符串
    std::string format_pattern_;
    // 格式化字符串解析后的解析器列表
    std::vector<FormatItem::sptr> format_items_;

public:
    explicit LogFormatter(const std::string &pattern={});
    std::string format(LogEvent::sptr ev);

private:
    void init();
};

//写
class LogAppender{
public:
    ADD_SPTR_DEFINE(LogAppender);
protected:
    LogLevel::Level level_;
    // 格式化器，将LogEvent对象格式化为指定的字符串格式
    LogFormatter::sptr formatter_;
    // formatter_'s lock
    std::mutex mutex_;

public:
    explicit LogAppender(LogLevel::Level level = LogLevel::Level::DEBUG);
    virtual ~LogAppender() = default;
    virtual void log(LogLevel::Level level, LogEvent::sptr ev) = 0;
    LogFormatter::sptr getFormatter() {
        std::unique_lock<std::mutex> lock(mutex_);
        return formatter_;
    }
    void setFormatter(const LogFormatter::sptr &formatter){
        std::unique_lock<std::mutex> lock(mutex_);
        formatter_ = formatter;
    }
};

class Logger{
public:
    ADD_SPTR_DEFINE(Logger);
private:
    const std::string name_;
    LogLevel::Level level_;
    std::string format_pattern_;
    // 日志默认格式化器
    LogFormatter::sptr formatter_;
    // Appender列表
    std::list<LogAppender::sptr> appenders_;
    std::mutex mutex_;

public:
    Logger();
    explicit Logger(const std::string &name,
                    const std::string &pattern,
                    LogLevel::Level level);
    virtual void log(LogEvent::sptr ev);
    void debug(LogEvent::sptr ev);
    void info(LogEvent::sptr ev);
    void warn(LogEvent::sptr ev);
    void error(LogEvent::sptr ev);
    void fatal(LogEvent::sptr ev);
    void addAppender(LogAppender::sptr appender);
    void delAppender(LogAppender::sptr appender);

    LogLevel::Level getLevet() const {
        return level_;
    }
    void setLevel(LogLevel::Level level) {
        level_ = level;
    }
};

class StdoutLogAppender : public LogAppender {
public:
    ADD_SPTR_DEFINE(StdoutLogAppender);
    StdoutLogAppender(LogLevel::Level level = LogLevel::Level::DEBUG);
    void log(LogLevel::Level level, LogEvent::sptr ev) override;
};

class FileLogAppender : public LogAppender {
public:
    ADD_SPTR_DEFINE(FileLogAppender);
    FileLogAppender(const std::string &file, LogLevel::Level level = LogLevel::Level::DEBUG);
    void log(LogLevel::Level level, LogEvent::sptr ev) override;
    bool reopen();

private:
    std::string file_name_;
    std::ofstream file_stream_;

private:

};

class LoggerManagerImp {
public:
    ADD_SPTR_DEFINE(LoggerManagerImp);
private:
    std::map<std::string, Logger::sptr> loggers_;
    std::mutex mutex_;
public:
    void init();

public:
    LoggerManagerImp();
    Logger::sptr getLogger(const std::string &name);
    Logger::sptr getGlobal();
    void ensureGlobalLoggerExist();
};

using LoggerManager = SignletonShare<LoggerManagerImp>; 

};


#endif 