#include "quicksort.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <thread>
#include <vector>

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
    int num_threads = argc > 2 ? std::stoi(argv[2]) : std::thread::hardware_concurrency();

    constexpr int size = 10000000;
    std::vector<int> vec;
    vec.reserve(size);

    std::ifstream in{"input"};

    fill_from_file(vec, in);

    omp_set_dynamic(0);
    omp_set_num_threads(num_threads);
   
    double begin = omp_get_wtime();
 
    #pragma omp parallel
    {
        #pragma omp single
        qsort_par(vec.begin(), vec.end());
    }

    double end = omp_get_wtime();

    std::cout << "Time: " << (end - begin) << " (s) - OpenMP\n";

    print_vec(vec);

    return 0;
}
