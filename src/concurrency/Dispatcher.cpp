#include "concurrency/Dispatcher.hpp"



void Dispatcher::Post(std::function<void()> task)
{
  std::lock_guard<std::mutex> lk(mutex_);
  tasks_.push(task);
}

void Dispatcher::Drain()
{
  std::queue<std::function<void()>> local;
  {
    std::lock_guard<std::mutex> lk(mutex_);
    tasks_.swap(local);
  }
  while(!local.empty())
  {
    auto task = local.front();
    task();
    local.pop();
  }
}
