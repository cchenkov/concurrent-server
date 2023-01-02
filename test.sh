#!/bin/bash

# CMake build
cmake -S . -B build > /dev/null
cmake --build build > /dev/null

# Execute
./build/omp_qsort_iter_test > out_omp && ./build/seq_qsort_iter_test > out_seq

# Compare exec time
diff out_omp out_seq | grep Time | cut -c 2-
