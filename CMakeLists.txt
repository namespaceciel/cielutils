cmake_minimum_required(VERSION 3.5)
project(cielutils VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (!CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    message(WARNING "So far only clang and gcc are supported.")
endif ()

add_library(cielutils INTERFACE)
target_include_directories(cielutils INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)

string(COMPARE EQUAL ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR} PROJECT_IS_TOP_LEVEL)

option(CIELUTILS_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})

if (CIELUTILS_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()