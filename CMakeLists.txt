cmake_minimum_required(VERSION 3.14...3.25)

project(
    ConcurrentServer
    VERSION 0.1
    DESCRIPTION "High performance concurrent server"
    LANGUAGES CXX
)

# Add fmt dependency
include(FetchContent)

FetchContent_Declare(
    fmtlib
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 5.3.0
)

FetchContent_MakeAvailable(fmtlib)

add_executable(seq_qsort_iter_test seq_qsort_iter_test.cpp)
add_executable(omp_qsort_iter_test omp_qsort_iter_test.cpp)
add_executable(tp_qsort_iter_test tp_qsort_test.cpp)
add_executable(seq_qsort_test seq_qsort_test.cpp)
add_executable(omp_qsort_test omp_qsort_test.cpp)
add_executable(tp_qsort_test tp_qsort_test.cpp)
add_executable(server server.cpp)
add_executable(client client.cpp)
add_executable(pack_test pack_test.cpp)

target_compile_features(seq_qsort_iter_test PRIVATE cxx_std_17)
target_compile_features(omp_qsort_iter_test PRIVATE cxx_std_17)
target_compile_features(tp_qsort_iter_test PRIVATE cxx_std_17)
target_compile_features(seq_qsort_test PRIVATE cxx_std_17)
target_compile_features(omp_qsort_test PRIVATE cxx_std_17)
target_compile_features(tp_qsort_test PRIVATE cxx_std_17)
target_compile_features(server PRIVATE cxx_std_17)
target_compile_features(client PRIVATE cxx_std_17)
target_compile_features(pack_test PRIVATE cxx_std_11)

find_package(Threads REQUIRED)
find_package(OpenMP)

target_link_libraries(tp_qsort_iter_test PRIVATE Threads::Threads)
target_link_libraries(tp_qsort_test PRIVATE Threads::Threads)
target_link_libraries(server PRIVATE Threads::Threads fmt::fmt)
target_link_libraries(client PRIVATE Threads::Threads fmt::fmt)

if(OpenMP_CXX_FOUND)
    target_link_libraries(omp_qsort_iter_test PUBLIC OpenMP::OpenMP_CXX)
    target_link_libraries(omp_qsort_test PUBLIC OpenMP::OpenMP_CXX)
endif()
