#include "monitor/cpu_load_monitor.h"

#include "utils/read_file.h"
#include "monitor_info.grpc.pb.h"
#include "monitor_info.pb.h"



namespace monitor {
void CpuLoadMonitor::UpdateOnce(monitor::proto::MonitorInfo* monitor_info) {
  ReadFile cpu_load_file(std::string("/proc/loadavg"));
  std::vector<std::string> cpu_load;
  cpu_load_file.ReadLine(&cpu_load);
  load_avg_1_ = std::stof(cpu_load[0]);
  load_avg_3_ = std::stof(cpu_load[1]);
  load_avg_15_ = std::stof(cpu_load[2]);

//获取正在运行的进程数和系统总进程数。
  std::istringstream tmp_if(cpu_load[3]);
  std::string tmp_s;
  std::getline(tmp_if, tmp_s, '/');
  int32_t processes_running =  std::stof(tmp_s);
  std::getline(tmp_if, tmp_s, '/');
  int32_t processes_total = std::stof(tmp_s);

  auto cpu_load_msg = monitor_info->mutable_cpu_load();
  cpu_load_msg->set_load_avg_1(load_avg_1_);
  cpu_load_msg->set_load_avg_3(load_avg_3_);
  cpu_load_msg->set_load_avg_15(load_avg_15_);

  return;
}


}  // namespace monitor