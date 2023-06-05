cmake_minimum_required(VERSION 3.18)
project(shfl)

add_subdirectory(src)
add_subdirectory(updater)
add_subdirectory(tests)
add_subdirectory(nativeAccess)

include(CTest)
add_test(suggest tests/test_suggest)