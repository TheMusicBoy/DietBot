#include "thread_pool.h"

namespace NDietBot {

TThreadPool::TThreadPool(size_t numThreads) {
    for (size_t i = 0; i < numThreads; ++i) {
        threads_.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(queueMutex_);
                    cv_.wait(lock, [this] {
                        return !tasks_.empty() || stop_;
                    });

                    if (stop_ && tasks_.empty()) {
                        return;
                    }

                    task = std::move(tasks_.front());
                    tasks_.pop();
                }
                task();
            }
        });
    }
}


TThreadPool::~TThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        stop_ = true;
    }

    cv_.notify_all();
    for (auto& thread : threads_) {
        thread.join();
    }
}

void TThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queueMutex_);
        tasks_.emplace(std::move(task));
    }
    cv_.notify_one();
}

} // namespace NDietBot
