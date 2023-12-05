#include "config.hpp"
#include <string>
#include <functional>
#include <iostream>



int main() {
    ClientConfigReader& clientConfig = 
        ClientConfigReader::getInstance("/work/test/config/client.yaml");
    std::vector<std::string> servers;
    std::string local;
    if (clientConfig.readServers(servers)) {
        std::cout << "servers :" << std::endl;
        for(auto &s : servers){
            std::cout << s << std::endl;
        }
    }
std::cout << "-------" << std::endl;
    ServerConfigReader& serverConfig = 
        ServerConfigReader::getInstance("/work/test/config/server.yaml");
    if (serverConfig.readSendTo(local)) {
        std::cout << "send to server : " << local << std::endl;
    }

    return 0;
}