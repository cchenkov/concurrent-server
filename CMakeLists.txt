cmake_minimum_required(VERSION 3.14...3.25)

project(
    ConcurrentServer
    VERSION 0.1
    DESCRIPTION "High performance concurrent server"
    LANGUAGES CXX
)

add_executable(server main.cpp)

target_compile_features(server PRIVATE cxx_std_17)
