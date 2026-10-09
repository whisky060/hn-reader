#include "concurrency/ThreadPool.hpp"
#include <cstddef>
#include <functional>
#include <mutex>
#include <thread>
#include <algorithm>

ThreadPool::ThreadPool(size_t numThreads)
{
  if(numThreads == 0 )
      numThreads = 1;
  while (threads_.size()<numThreads) {
    threads_.emplace_back(
      std::thread([this]{this->ThreadLoop();})
    );
  }
}
ThreadPool::~ThreadPool()
{
  {
    std::lock_guard<std::mutex> lk(queueMutex);
    stop_ = true;
  }
  cv_.notify_all();
  for(auto& th : threads_)
  {
    if(th.joinable())
      th.join();
  }
}

void ThreadPool::ThreadLoop()
{
  while(true)
  {


    std::function<void()> task;
    {
      std::unique_lock<std::mutex> lk(queueMutex);
      cv_.wait(lk,[this]{ return stop_ || !tasks_.empty();});
      if(stop_ || tasks_.empty())
        return;
      task = std::move(tasks_.front());
      tasks_.pop();
    }

    try
    {
      task();
    } catch (...){}
  }
}



std::size_t ThreadPool::DefaultThreads() {
    unsigned hc = std::thread::hardware_concurrency();
    return std::clamp(hc ? hc : 4u, 4u, 8u);
}
