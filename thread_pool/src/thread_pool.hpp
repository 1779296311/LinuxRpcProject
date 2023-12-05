
#ifndef THREAD_POOL_HPP_MONITOR
#define THREAD_POOL_HPP_MONITOR

#include <future>
#include <vector>
#include <memory>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <functional>
#include <utility>
namespace monitor{
class ThreadPool{
    public:
        ThreadPool(size_t wsize);
        ~ThreadPool();
    public:
        template<class F, class... Args>
        auto make_task(F&& f, Args&&... args) 
            -> std::future<std::result_of_t<F(Args...)>>;
    private:
        std::vector<std::thread> workers_;
        std::queue<std::function<void()>> tasks_;
        std::condition_variable cv_;
        std::mutex mutex_;
        bool stop_ {false};
};


template<class F, class... Args>
auto 
ThreadPool::make_task(F&& f, Args&&... args) 
-> std::future<std::result_of_t<F(Args...)>>{

    //写法二
    // auto task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    // using rt = decltype(task());
    // auto union_task = std::make_shared<std::packaged_task<rt()>> (
    //     std::move(task)
    // );

    using return_type = std::result_of_t<F(Args...)>;
    auto union_task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...)
    );

    {
        std::unique_lock<std::mutex> lock_(mutex_);
        if(stop_){
            throw std::runtime_error("make_task in stoped ThreadPool !\n");
        }
        tasks_.emplace([union_task](){ 
            (*union_task)();
        });
    }
    cv_.notify_one();
    return union_task->get_future();
}





}
#endif