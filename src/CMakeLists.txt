cmake_minimum_required(VERSION 3.22)
project(shuffle)

set(APP_SOURCES main.cpp ../include/shfl.h)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${PROJECT_NAME} ${APP_SOURCES})

add_subdirectory(cmd)
add_subdirectory(appmgr)
add_subdirectory(suggestion)
add_subdirectory(workspace)
add_subdirectory(utils)

add_dependencies(${PROJECT_NAME} commandmgr appmgr suggestion workspace utils)
target_link_libraries(${PROJECT_NAME} PRIVATE commandmgr appmgr suggestion workspace utils)

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

find_package(kubazip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE kubazip::kubazip)

target_link_libraries(${PROJECT_NAME} PRIVATE shflai)
target_include_directories(${PROJECT_NAME} PRIVATE ../shflai/include)

target_link_directories(${PROJECT_NAME} PRIVATE ../build/lib)

target_include_directories(${PROJECT_NAME} PRIVATE ../include ../include/cmd ../include/app ../include/suggestion ../include/workspace ../include/utils)