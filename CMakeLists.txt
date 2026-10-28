cmake_minimum_required(VERSION 3.14)
project(ProcessMonitor LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

# Parser code lives in a small library so the monitor and the tests share it
add_library(procparser src/Parser.cpp)
target_compile_options(procparser PRIVATE -Wall -Wextra)

add_executable(monitor src/main.cpp)
target_link_libraries(monitor PRIVATE procparser)
target_compile_options(monitor PRIVATE -Wall -Wextra)

# GoogleTest (downloaded on first configure)
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(parser_tests tests/test_parser.cpp)
target_link_libraries(parser_tests PRIVATE procparser GTest::gtest_main)
target_compile_options(parser_tests PRIVATE -Wall -Wextra)

include(GoogleTest)
gtest_discover_tests(parser_tests)
