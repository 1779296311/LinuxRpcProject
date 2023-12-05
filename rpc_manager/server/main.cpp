#include <iostream>
#include <grpc/grpc.h>
#include <grpcpp/server_builder.h>
#include "rpc_manager.h"

constexpr char kServerPortInfo[] = "0.0.0.0:50051";
void InitServer() {
  //创建了一个ServerBuilder对象用于配置和构建 gRPC 服务器。
  grpc::ServerBuilder builder;
  builder.AddListeningPort(kServerPortInfo, grpc::InsecureServerCredentials());

  monitor::GrpcManagerImpl grpc_server;
  //将 gRPC 服务注册到 ServerBuilder：
  builder.RegisterService(&grpc_server);

  //使用 BuildAndStart 方法构建并启动 gRPC 服务器
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  //阻塞等待 gRPC 服务器的关闭
  server->Wait();

  return;
}

int main() {
  InitServer();
  return 0;
}
