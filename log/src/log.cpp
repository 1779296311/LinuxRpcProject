#include "log.hpp"
#include <algorithm>
#include <ctime>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace monitor{
    
class PlainFormatItem : public LogFormatter::FormatItem {
public:
    explicit PlainFormatItem(const std::string &str) :
        str_(str) {}
    
    void format(std::ostream &out, LogEvent::sptr ev) override {
        out << str_;
    }
private:
    std::string str_;
};

class LevelFormatItem : public LogFormatter::FormatItem {
public:
    void format(std::ostream &out, LogEvent::sptr ev) override {
        out << LogLevel::levelToString(ev->getLevel());
    }
};

class LineFormatItem : public LogFormatter::FormatItem {
public:
    void format(std::ostream& out, LogEvent::sptr ev) override {
        out << ev->getLine();
    }
};

class FilenameFormatItem : public LogFormatter::FormatItem {
public:
    void format(std::ostream &out, LogEvent::sptr ev) override {
        out << ev->getFile();
    }
};

class ThreadIdFormatItem :  public LogFormatter::FormatItem {
public:
    void format(std::ostream &out, LogEvent::sptr ev) override {
        out << ev->getThreadId();
    }
};

class TimeFormatItem : public LogFormatter::FormatItem {
public:
    explicit TimeFormatItem(const std::string &tp = {"%Y-%m-%d %H:%M:%S"}) :
        time_pattern_(tp) {
        if(time_pattern_.empty()){
            time_pattern_ = std::string {"%Y-%m-%d %H:%M:%S"};
        }
    }

    void format(std::ostream &out, LogEvent::sptr ev) override {
        std::time_t time = ev->getTime();
        out << std::put_time(std::localtime(&time), time_pattern_.c_str());
    }
private:
    std::string time_pattern_;
};

class ContentFormatItem : public LogFormatter::FormatItem {
public: 
    void format(std::ostream& out, LogEvent::sptr ev) override {
        out << ev->getContent();
    }
};

class NewLineFormatItem : public LogFormatter::FormatItem {
public: 
    void format(std::ostream& out, LogEvent::sptr ev) override {
        out << std::endl;
    }
};

class PercentSignFormatItem : public LogFormatter::FormatItem {
public: 
    void format(std::ostream& out, LogEvent::sptr ev) override {
        out << '%';
    }
};

class TabFormatItem : public LogFormatter::FormatItem {
public:
    void format(std::ostream& out, LogEvent::sptr ev) override {
        out << '\t'; 
    }
};

/**
 * %p 输出日志等级
 * %f 输出文件名
 * %l 输出行号
 * %d 输出日志时间
 * %t 输出线程号
 * %m 输出日志消息
 * %n 输出换行
 * %% 输出百分号
 * %T 输出制表符
 * */
static std::map<char, LogFormatter::FormatItem::sptr> format_item_map{
#define MK(CHAR, ITEM) { CHAR, std::make_shared<ITEM>() }

    MK('p', LevelFormatItem),
    MK('f', FilenameFormatItem),
    MK('l', LineFormatItem),
    MK('d', TimeFormatItem),
    MK('t', ThreadIdFormatItem),
    MK('m', ContentFormatItem),
    MK('n', NewLineFormatItem),
    MK('%', PercentSignFormatItem),
    MK('T', TabFormatItem)

#undef MK
};

std::string LogLevel::levelToString(LogLevel::Level level){
    std::string result;
    switch (level){
    case Level::DEBUG :
        result = {"DEBUG"};
        break;
    case Level::INFO :
        result = {"INFO"};
        break;
    case Level::WARN :
        result = {"WARN"};
        break;
    case Level::ERROR :
        result = {"ERROR"};
        break;
    case Level::FATAL :
        result = {"FATAL"};
        break;
    default:
        result = {"UNKNOW"};
        break;
    }
    return result;
}

Logger::Logger() :
    name_("default"),
    level_(LogLevel::Level::DEBUG),
    format_pattern_({"[%d] [%p] [T:%t F:%F]%T%m%n"}) {
        formatter_ = std::make_shared<LogFormatter>(format_pattern_);
    }

Logger::Logger(const std::string &name,
               const std::string &pattern,
               LogLevel::Level level) :
        name_(name), level_(level), format_pattern_(pattern),
        formatter_(std::make_shared<LogFormatter>(pattern)) 
        {}

void
Logger::addAppender(LogAppender::sptr appender) {
    std::unique_lock<std::mutex> lock(mutex_);
    if(appender->getFormatter() == nullptr){
        appender->setFormatter(formatter_);
    }
    appenders_.push_back(appender);
}

void
Logger::delAppender(LogAppender::sptr appender){
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = 
        std::find(appenders_.begin(), appenders_.end(), appender);
    if(it != appenders_.end()) {
        appenders_.erase(it);
    }
}

void
Logger::log(LogEvent::sptr ev){
    if(ev->getLevel() < level_){
        return ;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    for(auto &appender : appenders_) {
        appender->log(ev->getLevel(), ev);
    }
}
void
Logger::debug(LogEvent::sptr ev){
    ev->setLevel(LogLevel::Level::DEBUG);
    log(ev);
}

void
Logger::info(LogEvent::sptr ev){
    ev->setLevel(LogLevel::Level::INFO);
    log(ev);
}

void
Logger::warn(LogEvent::sptr ev){
    ev->setLevel(LogLevel::Level::WARN);
    log(ev);
}

void
Logger::error(LogEvent::sptr ev){
    ev->setLevel(LogLevel::Level::ERROR);
    log(ev);
}
void
Logger::fatal(LogEvent::sptr ev){
    ev->setLevel(LogLevel::Level::FATAL);
    log(ev);
}


LogAppender::LogAppender(LogLevel::Level level) :
    level_(level) {}


StdoutLogAppender::StdoutLogAppender(LogLevel::Level level) :
    LogAppender(level) {}


void
StdoutLogAppender::log(LogLevel::Level level, LogEvent::sptr ev) {
    if(level < level_){
        return;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    std::cout << formatter_->format(ev);
    std::cout.flush();
}

FileLogAppender::FileLogAppender(const std::string &name, LogLevel::Level level) :
    file_name_(name), LogAppender(level) {
        reopen();
    }

void
FileLogAppender::log(LogLevel::Level level, LogEvent::sptr ev) {
    if(level < level_){
        return ;
    }
    std::unique_lock<std::mutex> lock(mutex_);
    file_stream_ << formatter_->format(ev);
    file_stream_.flush();
}

bool
FileLogAppender::reopen() {
    std::unique_lock<std::mutex> lock(mutex_);
    if(!file_stream_){
        file_stream_.close();
    }
    //写入模式， 追加模式
    file_stream_.open(file_name_, std::ios_base::out | std::ios_base::app);
    return !!file_stream_;
}

std::string
LogFormatter::format(LogEvent::sptr ev){
    std::stringstream ss;
    for(auto &fitem : format_items_){
        fitem->format(ss, ev);
    }
    return ss.str();
}

LogFormatter::LogFormatter(const std::string &pattern) :
    format_pattern_(pattern) {
        init();
}

void
LogFormatter::init() {
    enum class PARSE_STATUS {
        SCAN_STATUS,
        CREATE_STATUS
    };
    auto STATUS = PARSE_STATUS::SCAN_STATUS;
    for(size_t i=0; i<format_pattern_.length(); ++i){
        switch (STATUS){
            case PARSE_STATUS::SCAN_STATUS : {
                size_t start = i;
                for(; i<format_pattern_.length(); ++i){
                    if(format_pattern_[i] == '%'){
                        STATUS = PARSE_STATUS::CREATE_STATUS;
                        break;
                    }
                }
                format_items_.emplace_back(
                    std::make_shared<PlainFormatItem>(
                        format_pattern_.substr(start, i - start)
                    )
                );
                break;
            }

            case PARSE_STATUS::CREATE_STATUS : {
                auto it = format_item_map.find(format_pattern_[i]);
                if(it == format_item_map.end()){
                    format_items_.emplace_back(
                        std::make_shared<PlainFormatItem>(
                            std::string{"Error Format !"}
                        )
                    );
                }else{
                    format_items_.push_back(it->second);
                }
                STATUS = PARSE_STATUS::SCAN_STATUS;
                break;
            }
        }
    }   
}

LoggerManagerImp::LoggerManagerImp(){
    init();
}

void
LoggerManagerImp::ensureGlobalLoggerExist(){
    auto it = loggers_.find(std::string{"global"});
    if(it == loggers_.end()){
        // 默认的生成一个输出到 stdout 的 debug 级输出器
        auto global_logger = std::make_shared<Logger>();
        global_logger->addAppender(std::make_shared<StdoutLogAppender>());
        loggers_.insert(std::make_pair(std::string{"global"}, global_logger));
    }else if(it->second == nullptr){
        it->second = std::make_shared<Logger>();
        it->second->addAppender(std::make_shared<StdoutLogAppender>());
    }
}

void 
LoggerManagerImp::init(){
    // TODO 通过读取配置文件信息，创建需要的日志器
    std::unique_lock<std::mutex> lock(mutex_);
    auto log_info = monitor::Config::GetLogConfigFileInfo();
    auto logs_ptr = 
    monitor::Config::Register<std::vector<LogConfig>>(
        std::string{log_info.prefix_ + ".logs"}
    );
    if(monitor::Config::LoadFromFile(log_info.yaml_file_, log_info.prefix_)) {
        const auto &logs = logs_ptr->getValue();
        for(const auto &log : logs){
            loggers_.erase(log.name_);
            auto logger = std::make_shared<Logger>(
                log.name_, log.formatter_, log.level_
            );
            LogAppender::sptr logappd;
            for(const auto &appd : log.appenders_) {
                switch (appd.type_){
                    case LogAppenderConfig::Type::STDOUT : {
                        logappd = std::make_shared<StdoutLogAppender>(appd.level_);
                        break;
                    }
                    case LogAppenderConfig::Type::FILE : {
                        logappd = std::make_shared<FileLogAppender>(
                            appd.file_,
                            appd.level_
                        );
                        break;
                    }
                    default :{
                        std::cerr << "LoggerManager::init exception 无效的"
                                     "appender 配置值, appender.type=" 
                                  << std::endl;
                        break; 
                    }  
                }
                if(!appd.formatter_.empty()) {
                    logappd->setFormatter(
                        std::make_shared<LogFormatter>(appd.formatter_)
                    );
                }
                logger->addAppender(std::move(logappd));
            }
            loggers_.emplace(log.name_, std::move(logger));
        }
    }
    ensureGlobalLoggerExist();
}

Logger::sptr
LoggerManagerImp::getLogger(const std::string &name){
    std::unique_lock<std::mutex> lock(mutex_);
    auto it = loggers_.find(name);
    if(it == loggers_.end()){
        return loggers_.find("global")->second;
    }
    return it->second;
}

Logger::sptr
LoggerManagerImp::getGlobal(){
    std::unique_lock<std::mutex> lock(mutex_);
    return getLogger(std::string{"global"});
}
}