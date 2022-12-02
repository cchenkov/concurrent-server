#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include "safe_queue.h"

#include <iostream>
#include <functional>
#include <future>
#include <stdexcept>
#include <thread>
#include <tuple>
#include <vector>

class thread_pool {
public:
    explicit thread_pool(std::size_t thread_count = std::thread::hardware_concurrency()) {
        if (thread_count < 1) {
            throw std::invalid_argument("bad thread count - value must be non-zero");
        }

        auto worker = [this]() {
            while (true) {
                job_t job;
                
                if (!m_queue.pop(job)) {
                    break;
                }

                job();
            }
        };

        m_threads.reserve(thread_count);

        while (thread_count--) {
            m_threads.emplace_back(worker);
        }
    }

    ~thread_pool() {
        m_queue.unblock();

        for (auto& thread : m_threads) {
            thread.join();
        }
    }

    thread_pool(const thread_pool&) = delete;
    thread_pool(thread_pool&&) = delete;
    thread_pool& operator=(const thread_pool&) = delete;
    thread_pool& operator=(thread_pool&&) = delete;

    template <typename F, typename... Args>
    void enqueue_work(F&& f, Args&&... args) {
        m_queue.push([p = std::forward<F>(f), t = std::make_tuple(std::forward<Args>(args)...)]() { std::apply(p, t); });
    }

    template <typename F, typename... Args>
    auto enqueue_task(F&& f, Args&&... args) -> std::future<std::invoke_result_t<F, Args...>> {
        using task_return_type = std::invoke_result_t<F, Args...>;
        using task_type = std::packaged_task<task_return_type()>;

        auto task = std::make_shared<task_type>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
        auto result = task->get_future();

        m_queue.push([=]() { (*task)(); });

        return result;
    }

private:
    using job_t = std::function<void()>;
    using queue_t = safe_queue<job_t>;
    using threads_t = std::vector<std::thread>;

    queue_t m_queue;
    threads_t m_threads;
};

#endif // THREAD_POOL_H
