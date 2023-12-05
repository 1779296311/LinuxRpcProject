#include <iostream>
#include <fstream>
#include <yaml-cpp/yaml.h>

class ConfigReader {
public:
    const std::string& getFilePath() const {
        return file_path_;
    }
    ConfigReader(const std::string& file_path) : file_path_(file_path) {}
private:
    std::string file_path_;
};

class ClientConfigReader : public ConfigReader {
public:
    static ClientConfigReader& getInstance(const std::string& file_path) {
        static ClientConfigReader instance(file_path);
        return instance;
    }


    bool readServers(std::vector<std::string> &servers) {
        if(config_node_ == nullptr){
//            try {
                config_node_ = 
                    std::make_unique<YAML::Node>(YAML::LoadFile(getFilePath()));
            // }catch(const std::exception &e){
            //     //LOG
            //     return false;
            // }
        }
        // Access an array
        const YAML::Node& arrayNode = (*config_node_)["listening_servers"];
        for (const auto& element : arrayNode) {
            servers.push_back(element.as<std::string>());
        }
        return true;
    }

private:
    ClientConfigReader(const std::string& file_path) : ConfigReader(file_path) {}
    std::unique_ptr<YAML::Node> config_node_ {nullptr};
};

class ServerConfigReader : public ConfigReader {
public:
    static ServerConfigReader& getInstance(const std::string& file_path) {
        static ServerConfigReader instance(file_path);
        return instance;
    }


    bool readSendTo(std::string &local_hp) {
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

private:
    ServerConfigReader(const std::string& file_path) : ConfigReader(file_path) {}
    std::unique_ptr<YAML::Node> config_node_ {nullptr};
};

