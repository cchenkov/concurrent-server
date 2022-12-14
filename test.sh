#!/bin/bash

# CMake build
cmake -S . -B build > /dev/null
cmake --build build > /dev/null

# Execute
./build/omp_qsort_test > out_omp && ./build/tp_qsort_test > out_tp

# Compare exec time
diff out_omp out_tp | grep Time | cut -c 2-
