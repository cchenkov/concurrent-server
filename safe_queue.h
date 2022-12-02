#ifndef SAFE_QUEUE_H
#define SAFE_QUEUE_H

#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T>
class safe_queue {
public:
    explicit safe_queue(bool block = true) : m_block{block} {}
    ~safe_queue() = default;

    safe_queue(const safe_queue&) = delete;
    safe_queue(safe_queue&&) = delete;
    safe_queue& operator=(const safe_queue&) = delete;
    safe_queue& operator=(safe_queue&&) = delete;

    void push(const T& item) {
        {
            std::scoped_lock<std::mutex> lock(m_mutex);
            m_queue.push(item);
        }

        m_condition.notify_one();
    }

    void push(T&& item) {
        {
            std::scoped_lock<std::mutex> lock(m_mutex);
            m_queue.push(std::move(item));
        }

        m_condition.notify_one();
    }

    template <class... Args>
    void emplace(Args&&... args) {
        {
            std::scoped_lock<std::mutex> lock(m_mutex);
            m_queue.emplace(std::forward<Args>(args)...);
        }

        m_condition.notify_one();
    }

    bool try_push(T& item) {
        {
            std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);

            if (!lock) {
                return false;
            }

            m_queue.push(item);
        }

        m_condition.notify_one();
        return true;
    }

    bool try_push(T&& item) {
        {
            std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);

            if (!lock) {
                return false;
            }

            m_queue.push(std::move(item));
        }

        m_condition.notify_one();
        return true;
    }

    bool pop(T& item) {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [&]() { return !m_queue.empty() || !m_block; });
        
        if (m_queue.empty()) {
            return false;
        }

        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    bool try_pop(T &item) {
        std::unique_lock<std::mutex> lock(m_mutex, std::try_to_lock);

        if (!lock || m_queue.empty()) {
            return false;
        }

        item = std::move(m_queue.front());
        m_queue.pop();
        return true;
    }

    std::size_t size() const {
        std::scoped_lock<std::mutex> lock(m_mutex);
        return m_queue.size();
    }

    bool empty() const {
        std::scoped_lock<std::mutex> lock(m_mutex);
        return m_queue.empty();
    }

    void block() {
        std::scoped_lock<std::mutex> lock(m_mutex);
        m_block = true;
    }

    void unblock() {
        {
            std::scoped_lock<std::mutex> lock(m_mutex);
            m_block = false;
        }

        m_condition.notify_all();
    }

    bool blocking() const {
        std::scoped_lock<std::mutex> lock(m_mutex);
        return m_block;
    }

private:
    using queue_t = std::queue<T>;
    
    bool m_block;

    queue_t m_queue;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
};

#endif // SAFE_QUEUE_H
