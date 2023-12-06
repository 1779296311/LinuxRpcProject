#ifndef __SINGLETON_HPP_MONITOR
#define __SINGLETON_HPP_MONITOR

#include <memory>


namespace monitor{

template<class T>
class Singleton final {
public:
    static T* GetInstance(){
        static T instance;
        return &instance;
    }
    Singleton() = delete;
};

template<class T>
class SignletonShare final {
public:
    static std::shared_ptr<T> GetInstance(){
        static auto instance = std::make_shared<T>();
        return instance;
    }
    SignletonShare() = delete;
};

}

#endif