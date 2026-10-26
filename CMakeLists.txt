cmake_minimum_required(VERSION 3.14)
project(ProcessMonitor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(include)

add_executable(monitor src/main.cpp src/Parser.cpp)
target_link_libraries(monitor pthread)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(parser_tests tests/test_parser.cpp src/Parser.cpp)
target_link_libraries(parser_tests GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(parser_tests)
