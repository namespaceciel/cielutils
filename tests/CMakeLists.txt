cmake_minimum_required(VERSION 3.5)
project(cielutils_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE "Debug")

file(GLOB_RECURSE ALL_TESTS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
add_executable(cielutils_tests ${ALL_TESTS})

target_link_libraries(cielutils_tests cielutils)

add_subdirectory(googletest-1.14.0)

target_link_libraries(cielutils_tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(cielutils_tests)

target_compile_options(cielutils_tests PUBLIC -fexceptions -frtti -Wall -Wextra -Werror)