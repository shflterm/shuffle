cmake_minimum_required(VERSION 3.18)

project(cdCmd)

add_library(${PROJECT_NAME} SHARED
        entrypoint.cpp)

add_dependencies(${PROJECT_NAME} shuffle_lib)
target_include_directories(${PROJECT_NAME} PRIVATE ../../../include)
target_link_libraries(${PROJECT_NAME} shuffle_lib)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
