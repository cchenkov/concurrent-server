#include "quicksort.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>
#include <thread>

template <
    class result_t   = std::chrono::milliseconds,
    class clock_t    = std::chrono::steady_clock,
    class duration_t = std::chrono::milliseconds
>
auto since(std::chrono::time_point<clock_t, duration_t> const& start) {
    return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

template <typename T>
void print_arr(T *arr, std::size_t size) {
    for (std::size_t i = 0; i < size; i++) {
        std::cout << arr[i] << "\n";
    }

    std::cout << "\n";
}

void fill_random_array(int *arr, std::size_t size) {
    srand(time(0));

    for (std::size_t i = 0; i < size; i++) {
        arr[i] = (rand() % 10);
    }
}

void fill_from_file(int *arr, std::size_t size, std::istream &in) {
    for (std::size_t i = 0; i < size; i++) {
        in >> arr[i];
    }
}

int main(int argc, char *argv[]) {
    int num_threads = argc > 1 ? std::stoi(argv[1]) : std::thread::hardware_concurrency();

    constexpr int size = 1000000;
    int arr[size];

    std::ifstream in{"input"};

    fill_from_file(arr, size, in);

    ParallelQSort<int> pqs;
    
    auto start = std::chrono::steady_clock::now();
    
    pqs.sort(arr, size);
   
    auto end = std::chrono::steady_clock::now();

    std::cout << "Time: " 
              << std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count() 
              << " (s) - Thread Pool\n";

    print_arr(arr, size);

    return 0;
}
