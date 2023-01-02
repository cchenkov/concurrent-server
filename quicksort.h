#ifndef QUICKSORT_H
#define QUICKSORT_H

#include "thread_pool.h"

#include <cmath>
#include <vector>

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

template <typename RandomIt>
RandomIt partition(RandomIt first, RandomIt last) {
    auto i = first;
    auto j = std::prev(last);
    auto pivot = *(std::next(i, std::distance(i, j) / 2));
    
    if (*i < pivot) while (*++i < pivot);
    if (*j > pivot) while (*--j > pivot);

    while (i < j) {
        std::iter_swap(i, j);
        while (*++i < pivot);
        while (*--j > pivot);
    }

    return std::next(j);
}

template <typename RandomIt>
void qsort_seq(RandomIt first, RandomIt last) {
    if (std::distance(first, last) > 1) {
        RandomIt pivot = partition(first, last);
        qsort_seq(first, pivot);
        qsort_seq(pivot, last);
    }
}

template <typename RandomIt>
void qsort_par(RandomIt first, RandomIt last) {
    if (std::distance(first, last) > 1) {
        RandomIt pivot = partition(first, last);
        #pragma omp task if(std::distance(first, last) > TASK_LIMIT)
        qsort_seq(first, pivot);
        #pragma omp task if(std::distance(first, last) > TASK_LIMIT)
        qsort_seq(pivot, last);
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

            qsort_par_tp(arr, pivot_idx + 1, high);

            while (low_subpart.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
                tp.run_pending_task();
            }
        }
    }
    
private:
    thread_pool tp;
};

template <class RandomIt>
class ParallelQVSort {
public:
    ParallelQVSort() = default;

    void sort(RandomIt first, RandomIt last) {
        qsort_par_tp(first, last);
    }

private:
    void qsort_par_tp(RandomIt first, RandomIt last) {
        if (std::distance(first, last) > 1) {
            RandomIt pivot = partition(first, last);

            auto low_subpart = tp.enqueue_task(std::bind(&ParallelQVSort::qsort_par_tp, this, first, pivot));

            qsort_par_tp(pivot, last);

            while (low_subpart.wait_for(std::chrono::seconds(0)) == std::future_status::timeout) {
                tp.run_pending_task();
            }
        }
    }

private:
    thread_pool tp;
};

#endif // QUICKSORT_H
