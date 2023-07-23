cmake_minimum_required(VERSION 3.18)
project(unit_test)

add_executable(test_suggest suggest.cpp)
add_dependencies(test_suggest shuffle_lib)
target_link_libraries(test_suggest shuffle_lib)

add_executable(test_history history.cpp)
add_dependencies(test_history shuffle_lib)
target_link_libraries(test_history shuffle_lib)

include_directories(../include)

target_compile_features(test_suggest PRIVATE cxx_std_17)