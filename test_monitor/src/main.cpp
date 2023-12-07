
#include <memory>
#include <thread>
#include <vector>

#include "client/rpc_client.h"
#include "monitor/cpu_load_monitor.h"
#include "monitor/cpu_softirq_monitor.h"
#include "monitor/cpu_stat_monitor.h"
#include "monitor/mem_monitor.h"
#include "monitor/monitor_inter.h"
#include "monitor/net_monitor.h"

#include "monitor_info.grpc.pb.h"
#include "monitor_info.pb.h"
#include "config.hpp"

int main(int argc, char **argv) {
  std::vector<std::shared_ptr<monitor::MonitorInter>> runners_;
  runners_.emplace_back(new monitor::CpuSoftIrqMonitor());
  runners_.emplace_back(new monitor::CpuLoadMonitor());
  runners_.emplace_back(new monitor::CpuStatMonitor());
  runners_.emplace_back(new monitor::MemMonitor());
  runners_.emplace_back(new monitor::NetMonitor());


//读取配置信息
  std::string config_path {};
  if(argc < 1){
    config_path = std::string{"/work/configs.yml"};
  }else{
    config_path = argv[1];
  }
  monitor::Config::READ_GLOBAL_FIEL_INFORMATION(
    config_path
  );
  auto client_info = monitor::Config::GetServerConfigFileInfo();

  monitor::RpcClientSender rpc_client_(client_info.yaml_file_, client_info.prefix_);
  char *name = getenv("USER");
  std::unique_ptr<std::thread> thread_ = nullptr;
  thread_ = std::make_unique<std::thread>([&]() {
    while (true) {
      //这里不能用循环外面创建的方式， 
      //因为UpdateOnce里面会调用add_方法， 给数组添加元素
      //那样的话，会一直增加空间， 会溢出的；
      //倒是可以调用， clear方法
      monitor::proto::MonitorInfo monitor_info;
      monitor_info.set_name(std::string(name));
      for (auto& runner : runners_) {
        runner->UpdateOnce(&monitor_info);
      }

      rpc_client_.SetMonitorInfo(monitor_info);
      std::this_thread::sleep_for(std::chrono::seconds(3));
    }
  });

  thread_->join();
  return 0;
}
