#pragma once

#include <grpc/grpc.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/grpcpp.h>

#include "monitor_info.grpc.pb.h"
#include "monitor_info.pb.h"
#include <vector>

namespace monitor {


class RpcClient {
 public:
  RpcClient();
  ~RpcClient();
};

class RpcClientReceiver : public RpcClient{
  public:
    RpcClientReceiver();
    ~RpcClientReceiver();

  public:
    using ptr = std::unique_ptr<monitor::proto::GrpcManager::Stub>;

  public:
    void GetMonitorInfo(std::vector<monitor::proto::MonitorInfo>& monito_infos);
    size_t GetServerSize() const {
      return size_;
    }
    const std::vector<std::string>& GetServerAddr() const {
      return servers;
    }
  private:
    size_t size_ {0} ; 
    std::vector<ptr> stub_ptrs_;
    std::vector<std::string> servers;
};

class RpcClientSender : public RpcClient{
  public:
    RpcClientSender();
    ~RpcClientSender();
  public:
    void SetMonitorInfo(const monitor::proto::MonitorInfo& monito_info);
  private:
    std::unique_ptr<monitor::proto::GrpcManager::Stub> stub_ptr_;
};

}  // namespace monitor
