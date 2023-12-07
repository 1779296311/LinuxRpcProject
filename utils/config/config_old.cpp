#include "config.hpp"
namespace monitor {


bool 
ClientConfigReader::readServers
(std::vector<std::string> &servers) {
    if(config_node_ == nullptr){
        try {
            config_node_ = 
                std::make_unique<YAML::Node>(YAML::LoadFile(getFilePath()));
        }catch(const std::exception &e){
            //LOG
            return false;
        }
    }
    // Access an array
    const YAML::Node& arrayNode = (*config_node_)["listening_servers"];
    for (const auto& element : arrayNode) {
        servers.push_back(element.as<std::string>());
    }
    return true;
}
bool
ServerConfigReader::
readSendTo(std::string &local_hp) {
    if(config_node_ == nullptr){
        try {
            config_node_ = 
                std::make_unique<YAML::Node>(YAML::LoadFile(getFilePath()));
        }catch(const std::exception &e){
            //LOG
            return false;
        }
    }
    local_hp = (*config_node_)["send_to_server"].as<std::string>();
    return true;
}

}