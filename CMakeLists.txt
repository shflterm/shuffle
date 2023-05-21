cmake_minimum_required(VERSION 3.18)
project(shfl)

add_subdirectory(src)
add_subdirectory(${CMAKE_SOURCE_DIR}/commands)
add_subdirectory(tests)

include(CTest)
add_test(suggest tests/test_suggest)