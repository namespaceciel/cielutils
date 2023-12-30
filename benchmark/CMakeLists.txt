cmake_minimum_required(VERSION 3.5)
project(cielutils_benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Release")

add_executable(cielutils_benchmark
        src/all.cpp
        src/sort_benchmarks.cpp
        src/vector_benchmarks.cpp
        src/deque_benchmarks.cpp
        src/set_benchmarks.cpp
)

target_link_libraries(cielutils_benchmark cielutils benchmark::benchmark EASTL)

add_subdirectory(benchmark-1.8.3)
add_subdirectory(EASTL-3.21.12)

target_compile_options(cielutils_benchmark PUBLIC -O3 -fexceptions -frtti)