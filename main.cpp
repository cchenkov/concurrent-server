#include "thread_pool.h"

#include <iostream>

int main() {
    std::mutex out_mutex;

    std::cout << "main thread id: " << std::this_thread::get_id() << "\n";

    thread_pool tp;

    for (auto i = 1; i <= 1000; i++) {
        tp.enqueue_work([&, i = i]() {
            std::unique_lock<std::mutex> lock(out_mutex);
            std::cout << "doing work " << i << "...\n";
        });
    }

    return 0;
}
