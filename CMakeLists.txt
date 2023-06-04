cmake_minimum_required(VERSION 3.18)
project(shfl)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(termcpp)

add_subdirectory(src)
add_subdirectory(updater)
add_subdirectory(${CMAKE_SOURCE_DIR}/commands)
add_subdirectory(tests)

include(CTest)
add_test(suggest tests/test_suggest)