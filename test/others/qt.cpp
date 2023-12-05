
// #include <QApplication>
// #include <QWidget>
// #include <QTabWidget>
// #include <QStackedWidget>
// #include <QPushButton>
// #include <QGridLayout>

// class MonitoringPage : public QWidget {
// public:
//     MonitoringPage(QWidget* parent = nullptr) : QWidget(parent) {
//         // 创建标签页
//         tabWidget_ = new QTabWidget(this);
//         stackedWidget_ = new QStackedWidget(this);

//         // 设置布局
//         QGridLayout* layout = new QGridLayout(this);
//         layout->addWidget(tabWidget_, 0, 0, 1, 2);
//         layout->addWidget(stackedWidget_, 1, 0, 1, 2);

//         setLayout(layout);

//         // 添加页面
//         for (int i = 0; i < pageCount; ++i) {
//             addPage(i);
//         }

//         // 连接标签切换信号与槽
//         connect(tabWidget_, &QTabWidget::currentChanged, this, &MonitoringPage::changePage);
//     }

// private:
//     static const int pageCount = 3; // 设置页面数量

//     QTabWidget* tabWidget_;
//     QStackedWidget* stackedWidget_;

//     // 初始化页面，这里用 QPushButton 代替你的监控部件
//     QWidget* initPageWidget(int index) {
//         QWidget* page = new QWidget(this);
//         QPushButton* button = new QPushButton(QString("Page %1").arg(index + 1), page);
//         QVBoxLayout* layout = new QVBoxLayout(page);
//         layout->addWidget(button);
//         page->setLayout(layout);
//         return page;
//     }

//     // 添加页面
//     void addPage(int index) {
//         QWidget* page = initPageWidget(index);
//         stackedWidget_->addWidget(page);
//         tabWidget_->addTab(page, QString("Page %1").arg(index + 1));
//     }

//     // 切换页面
//     void changePage(int index) {
//         stackedWidget_->setCurrentIndex(index);
//     }
// };

// int main(int argc, char *argv[]) {
//     QApplication app(argc, argv);

//     MonitoringPage mainWindow;
//     mainWindow.show();

//     return app.exec();
// }
#include <iostream>
#include <string>

class Singleton {
public:
    // 获取单例实例的静态方法
    static Singleton& instance(const std::string& param) {
        static Singleton instance(param);
        return instance;
    }

    // 打印参数的方法
    void printParam() const {
        std::cout << "Param: " << param_ << std::endl;
    }

private:
    // 私有构造函数，防止外部直接实例化
    Singleton(const std::string& param) : param_(param) {}

    std::string param_;
};

int main() {
    // 获取单例实例，并传入不同的参数
    Singleton& instance1 = Singleton::instance("String1");
    Singleton& instance2 = Singleton::instance("String2");

    // 输出参数
    instance1.printParam();  // 应该输出 "String1"
    instance2.printParam();  // 应该输出 "String1"

    return 0;
}
