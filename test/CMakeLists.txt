cmake_minimum_required(VERSION 3.5)
project(cielutils_test LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")

add_executable(cielutils_test
        src/all.cpp
        src/algorithm_tests.cpp
        src/any_tests.cpp
        src/array_tests.cpp
        src/concepts_tests.cpp
        src/deque_tests.cpp
        src/forward_list_tests.cpp
        src/function_tests.cpp
        src/iterator_tests.cpp
        src/list_tests.cpp
        src/map_tests.cpp
        src/memory_tests.cpp
        src/pair_tests.cpp
        src/queue_tests.cpp
        src/set_tests.cpp
        src/span_tests.cpp
        src/split_buffer_tests.cpp
        src/stack_tests.cpp
        src/tuple_tests.cpp
        src/type_traits_tests.cpp
        src/unordered_set_tests.cpp
        src/variant_tests.cpp
        src/vector_tests.cpp
        src/exception_safety_tests.cpp
)

target_link_libraries(cielutils_test cielutils)

add_subdirectory(googletest-1.14.0)

target_link_libraries(cielutils_test GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(cielutils_test)

target_compile_options(cielutils_test PUBLIC -fexceptions -frtti -Wall -Wextra -Werror)