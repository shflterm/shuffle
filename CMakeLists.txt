cmake_minimum_required(VERSION 3.22)
project(shfl)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(termcpp)

add_subdirectory(src)
add_subdirectory(updater)

include(CTest)
add_test(suggest tests/test_suggest)