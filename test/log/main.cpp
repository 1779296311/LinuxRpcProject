#include "log.hpp"

auto logs = 
    monitor::Config::Register<std::vector<monitor::LogConfig>>("log_config.logs");


int main() {

    // monitor::Config::LoadFromFile(std::string{"/work/test/log/log_config.yml"}, 
    //                               std::string{"log_config"});

    // auto value = logs->getValue();
    // for(auto &v : value) {
    //     std::cout << v.name_ << " -- " << v.formatter_ << std::endl;
    //     std::cout << "AppenderSize : " << v.appenders_.size() << std::endl;
    //     for(auto &appd : v.appenders_){
    //         std::cout << "appender: "  << std::endl;
    //         std::cout << "\t\t\t" << appd.formatter_ << std::endl;
    //         std::cout << "\t\t\t" << appd.file_ << std::endl;
    //     }
    // }
  monitor::Config::READ_GLOBAL_FIEL_INFORMATION(
    std::string{"/work/configs.yml"}
  );
  auto client_info = monitor::Config::GetClientConfigFileInfo();
  std::cout << client_info.yaml_file_ << "  " << client_info.prefix_ << std::endl;
  monitor::Config::DEBUG();
  return 0;
}