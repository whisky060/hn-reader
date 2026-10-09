#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <condition_variable>
#include <functional>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
  public:
    ThreadPool(size_t numThreads = DefaultThreads());
    ~ThreadPool();
    template <class F>
    [[nodiscard]] auto Submit(F&& f)
        -> std::future<std::invoke_result_t<std::decay_t<F>>>
        {
            using R = std::invoke_result_t<std::decay_t<F>>;
            // packaged_task is move-only; shared_ptr lets it live inside std::function
            auto task = std::make_shared<std::packaged_task<R()>>(std::forward<F>(f));
            std::future<R> fut = task->get_future();
            {
                std::lock_guard lk(queueMutex);
                if (stop_)
                    throw std::runtime_error("ThreadPool::Submit after stop");
                tasks_.emplace([task] { (*task)(); });
            }
            cv_.notify_one();
            return fut;
        }


  private:
    void ThreadLoop();
    static size_t DefaultThreads();
    std::condition_variable cv_;
    std::vector<std::thread> threads_;
    std::queue<std::function<void()>> tasks_;
    bool stop_{false};
    std::mutex queueMutex;
};


#endif
