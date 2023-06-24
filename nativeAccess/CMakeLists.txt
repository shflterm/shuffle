cmake_minimum_required(VERSION 3.22)
project(nativeAccess)

set(CMAKE_CXX_STANDARD 17)

add_library(nativeAccess SHARED library.cpp)

find_package(Lua CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE ../include)
target_link_libraries(${PROJECT_NAME} shuffle_lib)
