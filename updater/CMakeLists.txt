cmake_minimum_required(VERSION 3.22)
project(updater)

set(SOURCES ../include/cmd/job.h)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${PROJECT_NAME} ${SOURCES} main.cpp)

add_dependencies(${PROJECT_NAME} utils appmgr)
target_link_libraries(${PROJECT_NAME} PRIVATE utils appmgr)

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

find_package(kubazip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE kubazip::kubazip)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_include_directories(${PROJECT_NAME} PRIVATE ../include)