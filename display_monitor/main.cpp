#pragma once

#include <QApplication>
#include <thread>
#include <chrono>
#include <QApplication>
#include <condition_variable>
#include "client/rpc_client.h"
#include "monitor_widget.h"

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  //设置样式
  QFile styleFile("/work/display_monitor/QSS/Now.qss");
  styleFile.open(QFile::ReadOnly);
  QString style = QLatin1String(styleFile.readAll());
  app.setStyleSheet(style);

  //用于与 RPC 服务器通信
  monitor::RpcClientReceiver rpc_client;
  //用于存储监视信息
  std::vector<monitor::proto::MonitorInfo> monitor_infos(rpc_client.GetServerSize());

  rpc_client.GetMonitorInfo(monitor_infos);
  std::cout << monitor_infos.size() << std::endl;
  auto monitor = monitor::MonitorPage(rpc_client.GetServerSize(), rpc_client.GetServerAddr());

  monitor.UpdateData(monitor_infos);
  monitor.show();


  std::atomic<bool> shouldExit(false);
  std::mutex exitMutex;
  std::condition_variable exitCondition;

  std::unique_ptr<std::thread> thread_ = 
    std::make_unique<std::thread>([&]() {
      bool exit_ = false;
      while (!exit_) {
        rpc_client.GetMonitorInfo(monitor_infos);
        monitor.UpdateData(monitor_infos);
        std::unique_lock<std::mutex> lock(exitMutex);
        //防止过多等待， 比如在第一秒就结束了， 就不用等第二秒了；
        exit_ = exitCondition.wait_for
            (lock, std::chrono::seconds(2), [&shouldExit](){return shouldExit.load();});
      }
  });


  //如果主窗口被关闭，事件循环将结束，app.exec() 返回，程序随之退出
  auto result = app.exec();
  // 设置退出标志，等待子线程完成
  {
    std::lock_guard<std::mutex> lock(exitMutex);
    shouldExit = true;
    exitCondition.notify_one();
  }
  thread_->join();
  return result;
}


