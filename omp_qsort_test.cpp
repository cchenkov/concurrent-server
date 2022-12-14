#include "quicksort.h"

#include <iostream>
#include <fstream>
#include <ctime>
#include <thread>

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
    int num_threads = argc > 2 ? std::stoi(argv[2]) : std::thread::hardware_concurrency();

    constexpr int size = 100000;
    int arr[size];

    std::ifstream in{"input"};

    fill_from_file(arr, size, in);

    omp_set_dynamic(0);
    omp_set_num_threads(num_threads);
   
    double begin = omp_get_wtime();
    
    #pragma omp parallel
    {
        #pragma omp single
        qsort_par(arr, 0, size - 1);
    }

    double end = omp_get_wtime();

    std::cout << "Time: " << (end - begin) << " (s) - OpenMP\n";

    print_arr(arr, size);

    return 0;
}
