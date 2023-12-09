cmake_minimum_required(VERSION 3.22)
project(shflai)

set(AI_SRC shflai.cpp)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} ${AI_SRC})

install(TARGETS ${PROJECT_NAME} RUNTIME)
target_link_libraries(${PROJECT_NAME} PRIVATE common llama ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} PRIVATE ../../llama.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ../include)