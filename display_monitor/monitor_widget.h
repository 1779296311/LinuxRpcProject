#pragma once

#include <QStandardItemModel>
#include <QtWidgets>
#include <QApplication>
#include <QWidget>
#include <QTabWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <QGridLayout>
#include <thread>
#include <string>
#include <vector>

#include "cpu_load_model.h"
#include "cpu_softirq_model.h"
#include "cpu_stat_model.h"
#include "mem_model.h"
#include "net_model.h"

#include "monitor_info.grpc.pb.h"
#include "monitor_info.pb.h"

#include "thread_pool.hpp"


namespace monitor {
class MonitorWidget : public QWidget {
  Q_OBJECT
 public:
  explicit MonitorWidget(QWidget* parent = nullptr);
  ~MonitorWidget() {
    // stop_ = true;
    // if (thread_->joinable()) {
    //   thread_->join();
    // }
  }
  QWidget* ShowAllMonitorWidget(const std::string& name);
  QWidget* InitCpuMonitorWidget();
  QWidget* InitSoftIrqMonitorWidget();
  QWidget* InitMemMonitorWidget();
  QWidget* InitNetMonitorWidget();
  QWidget* InitButtonMenu(const std::string& name);
  // QWidget* InitMemMonitorWidget();

  void UpdateData(const monitor::proto::MonitorInfo& monitor_info);

 private slots:
  void ClickCpuButton();
  void ClickSoftIrqButton();
  void ClickMemButton();
  void ClickNetButton();

 private:
  QTableView* monitor_view_ = nullptr;
  QTableView* cpu_load_monitor_view_ = nullptr;
  QTableView* cpu_stat_monitor_view_ = nullptr;
  QTableView* mem_monitor_view_ = nullptr;
  QTableView* net_monitor_view_ = nullptr;

  MonitorBaseModel* monitor_model_ = nullptr;
  CpuLoadModel* cpu_load_model_ = nullptr;
  CpuStatModel* cpu_stat_model_ = nullptr;
  MemModel* mem_model_ = nullptr;
  NetModel* net_model_ = nullptr;

  QStackedLayout* stack_menu_ = nullptr;

  // std::unique_ptr<std::thread> thread_;
  // bool stop = false;
};

class MonitorPage : public QWidget{
Q_OBJECT
public:

public:
  MonitorPage
    (size_t size, const std::vector<std::string>& hps, QWidget* parent = nullptr) 
    : size_(size), QWidget(parent){

    widgets_.resize(size_);
    // 创建标签页
    tabWidget_ = new QTabWidget(this);
    stackedWidget_ = new QStackedWidget(this);

    // 设置布局
    QGridLayout* layout = new QGridLayout(this);
    layout->addWidget(tabWidget_, 0, 0, 1, 2);
    layout->addWidget(stackedWidget_, 1, 0, 1, 2);

    setLayout(layout);

    // 添加页面
    for (size_t i = 0; i < size_; ++i) {
      widgets_[i] = new monitor::MonitorWidget {};
      QWidget* page = widgets_[i]->ShowAllMonitorWidget("info");
      stackedWidget_->addWidget(page);
      tabWidget_->addTab(page, QString("%1").arg( hps[i].substr(0, hps[i].find(':')).c_str() ));
    }

    // 连接标签切换信号与槽
    connect(tabWidget_, &QTabWidget::currentChanged, this, &MonitorPage::changePage);

    //设置线程池
    threadpool_ = std::make_unique<ThreadPool>(size_); 
  }
    // 切换页面
  void changePage(int index) {
      stackedWidget_->setCurrentIndex(index);
  }
  void SetSize(size_t size) {
    if(size == size_){
      return;
    }
    size_ = size;
    widgets_.resize(size_);
    threadpool_ = std::make_unique<ThreadPool>(size_);
  }
  // void UpdateData(const std::vector<monitor::proto::MonitorInfo> &monitor_infos){
  //   size_t size = std::min(size_, monitor_infos.size());
  //   for(size_t i=0; i<size; ++i){
  //     widgets_[i]->UpdateData(monitor_infos[i]);
  //   }
  // }
  void UpdateData(const std::vector<monitor::proto::MonitorInfo> &monitor_infos){
    size_t size = std::min(size_, monitor_infos.size());
    std::vector<std::future<void>> tmp_res;
    for(size_t i=0; i<size; ++i){
      tmp_res.emplace_back(
        threadpool_->make_task([](MonitorWidget* mo, const monitor::proto::MonitorInfo &info){
          return mo->UpdateData(info);
        },
        widgets_[i],
        monitor_infos[i]
        )
      );
    }
    for(auto &x : tmp_res){
      x.get();
    }
  }

private:
  size_t size_ {0};
  std::vector<MonitorWidget*> widgets_;
  QTabWidget* tabWidget_;
  QStackedWidget* stackedWidget_;
  std::unique_ptr<monitor::ThreadPool> threadpool_{nullptr};
};

}  // namespace monitor
