#include "rpc_client.h"
#include "config.hpp"


namespace monitor {
RpcClient::RpcClient() { }
RpcClient::~RpcClient() { }

RpcClientReceiver::RpcClientReceiver() {

  auto& clientConfig = ClientConfigReader::getInstance("/work/client.yaml");
  if (!clientConfig.readServers(servers)) {
    servers.emplace_back(std::string{"localhost:50051"});
  }
  for(int i=0; i<servers.size(); ++i){
    //指定主机端口，并且不加密
    auto channel = grpc::CreateChannel(servers[i], grpc::InsecureChannelCredentials());
    //初始化 gRPC 存根。gRPC 存根是用于客户端调用 gRPC 服务端提供的 RPC
    stub_ptrs_.push_back(monitor::proto::GrpcManager::NewStub(channel));
  }
  size_ = stub_ptrs_.size();
}
RpcClientReceiver::~RpcClientReceiver() {}

RpcClientSender::RpcClientSender() {

  auto& serverConfig = ServerConfigReader::getInstance("/work/server.yaml");
  std::string local;
  if (!serverConfig.readSendTo(local)) {
    local = {"localhost:50051"};
    return;
  }
  //指定主机端口，并且不加密
  auto channel = 
    grpc::CreateChannel(local,grpc::InsecureChannelCredentials());
  //初始化 gRPC 存根。gRPC 存根是用于客户端调用 gRPC 服务端提供的 RPC
  stub_ptr_ = monitor::proto::GrpcManager::NewStub(channel);
}
RpcClientSender::~RpcClientSender() {}


  //该 RPC 方法用于向服务端请求获取监控信息，允许客户端获取服务端当前的监控状态
void RpcClientReceiver::GetMonitorInfo(std::vector<monitor::proto::MonitorInfo>& monito_infos) {
  int i = 0;
  for(auto &stub_ptr_ : stub_ptrs_){
    ::grpc::ClientContext context;
    ::google::protobuf::Empty request;
    monito_infos[i].Clear();
    auto status =
        stub_ptr_->GetMonitorInfo(&context, request, &monito_infos[i++]);
    if (!status.ok()) {
      std::cout << status.error_details() << std::endl;
      std::cout << "falied to connect !!!" << std::endl;
    }
  }
}

  //该 RPC 方法用于向服务端发送监控信息，允许客户端更新或设置监控信息
void RpcClientSender::SetMonitorInfo(const monitor::proto::MonitorInfo& monito_info) {
  ::grpc::ClientContext context;
  ::google::protobuf::Empty response;
  //此函数定义在 proto中
  ::grpc::Status status =
      stub_ptr_->SetMonitorInfo(&context, monito_info, &response);
  if (status.ok()) {
  } else {
    std::cout << status.error_details() << std::endl;
    std::cout << "falied to connect !!!" << std::endl;
  }
}

}  // namespace monitor

