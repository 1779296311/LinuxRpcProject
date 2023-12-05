#include "thread_pool.hpp"
namespace monitor{
    
ThreadPool::~ThreadPool(){
    {
        std::unique_lock<std::mutex> lock_(mutex_);
        stop_ = true;
    }
    cv_.notify_all();
    for(auto &worker  : workers_){
        worker.join();
    }
}

ThreadPool::ThreadPool(size_t wsize) {
    for(size_t i=0; i<wsize; ++i){
        workers_.emplace_back(
            [this](){
                for(;;){
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock_(this->mutex_);
                        this->cv_.wait(lock_, [this]()
                                {return this->stop_ || !this->tasks_.empty();});
                        if(this->stop_ && this->tasks_.empty()){
                            return;
                        }
                        task = std::move(this->tasks_.front());
                        this->tasks_.pop();
                    }
                    task();
                }
            }
        );
    }
}

}