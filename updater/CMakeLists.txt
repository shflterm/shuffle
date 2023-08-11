cmake_minimum_required(VERSION 3.18)
project(updater)

set(SOURCES ../include/cmd/parsedcmd.h)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${PROJECT_NAME} ${SOURCES} main.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)

add_dependencies(${PROJECT_NAME} shuffle)
target_include_directories(${PROJECT_NAME} PRIVATE ../include)
target_link_libraries(${PROJECT_NAME} shuffle_lib)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} CURL::libcurl)

find_package(kubazip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} kubazip::kubazip)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME} PRIVATE ../include ../include/cmd ../include/sapp ../include/suggestion ../include/workspace ../include/utils)