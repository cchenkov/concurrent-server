#include "quicksort.h"

#include <iostream>
#include <fstream>
#include <chrono>
#include <ctime>

template <
    class result_t   = std::chrono::milliseconds,
    class clock_t    = std::chrono::steady_clock,
    class duration_t = std::chrono::milliseconds
>
auto since(std::chrono::time_point<clock_t, duration_t> const& start) {
    return std::chrono::duration_cast<result_t>(clock_t::now() - start);
}

template <typename T>
void print_vec(std::vector<T> &vec) {
    for (T elem : vec) {
        std::cout << elem << "\n";
    }

    std::cout << "\n";
}

void fill_random_array(int *arr, std::size_t size) {
    srand(time(0));

    for (std::size_t i = 0; i < size; i++) {
        arr[i] = (rand() % 10);
    }
}

void fill_from_file(std::vector<int> &vec, std::istream &in) {
    for (std::size_t i = 0; i < vec.capacity(); i++) {
        int n;
        in >> n;
        vec.push_back(n);
    }
}

int main(int argc, char *argv[]) {
    constexpr int size = 1000000;
    std::vector<int> vec;
    vec.reserve(size);

    std::ifstream in{"input"};

    fill_from_file(vec, in);

    auto start = std::chrono::steady_clock::now();
    
    qsort_seq(vec.begin(), vec.end());
   
    auto end = std::chrono::steady_clock::now();

    std::cout << "Time: " 
              << std::chrono::duration_cast<std::chrono::duration<double>>(end - start).count() 
              << " (s) - Sequential\n";

    print_vec(vec);

    return 0;
}
