#ifndef DISPATCHER_H
#define DISPATCHER_H

#include <functional>
#include <mutex>
#include <queue>


class Dispatcher
{

  public:
    Dispatcher() = default;
    ~Dispatcher() = default;
    void Post(std::function<void()> task);
    void Drain();

  private:
    std::queue<std::function<void()>> tasks_;
    std::mutex mutex_;

};


#endif
