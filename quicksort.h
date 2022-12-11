#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "thread_pool.h"

#include <cmath>

#include <omp.h>

#define TASK_LIMIT 100

template <typename T>
int partition(T *arr, int low, int high) {
    int pivot = arr[static_cast<int>(std::floor((high + low) / 2))];
    int i = low - 1;
    int j = high + 1;

    while (true) {
        do i++; while (arr[i] < pivot);
        do j--; while (arr[j] > pivot);

        if (i >= j) return j;

        std::swap(arr[i], arr[j]);
    }
}

template <typename T>
void qsort_seq(T *arr, int low, int high) {
    if (low >= 0 && high >= 0 && low < high) {
        int pivot_idx = partition(arr, low, high);
        qsort_seq(arr, low, pivot_idx);
        qsort_seq(arr, pivot_idx + 1, high);
    }
}

template <typename T>
void qsort_par(T *arr, int low, int high) {
    if (low >= 0 && high >= 0 && low < high) {
        int pivot_idx = partition(arr, low, high);
        #pragma omp task shared(arr) if(high - low > TASK_LIMIT)
        qsort_par(arr, low, pivot_idx);
        #pragma omp task shared(arr) if(high - low > TASK_LIMIT)
        qsort_par(arr, pivot_idx + 1, high);
    }
}

template <class T>
class ParallelQSort {
public:
    ParallelQSort() = default;

    void sort(T *arr, int size) {
        qsort_par_tp(arr, 0, size - 1);
    }
    
private:
    void qsort_par_tp(T *arr, int low, int high) {
        if (low >= 0 && high >= 0 && low < high) {
            int pivot_idx = partition(arr, low, high);

            auto low_subpart = tp.enqueue_task(std::bind(&ParallelQSort::qsort_par_tp, this, arr, low, pivot_idx));

            qsort_par(arr, pivot_idx + 1, high);

            while (low_subpart.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
                tp.run_pending_task();
            }

            low_subpart.get();
        }
    }

private:
    thread_pool tp;
};

#endif // QUICKSORT_H
