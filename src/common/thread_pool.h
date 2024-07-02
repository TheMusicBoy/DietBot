#pragma once

#include <thread>
#include <mutex>
#include <functional>
#include <queue>

namespace NDietBot {

class TThreadPool {
public:
    TThreadPool(size_t numThreads = std::thread::hardware_concurrency());
    ~TThreadPool();

    void enqueue(std::function<void()> task);

private:
    std::vector<std::thread> threads_;
    std::queue<std::function<void()> > tasks_;
    std::mutex queueMutex_;
    std::condition_variable cv_;
    bool stop_ = false;

};

using TThreadPoolPtr = std::shared_ptr<TThreadPool>;

} // namespace NDietBot
