cmake_minimum_required(VERSION 3.22)
project(shuffle)

set(APP_SOURCES main.cpp ../include/shfl.h)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${PROJECT_NAME} ${APP_SOURCES})

add_subdirectory(ai)
add_subdirectory(appmgr)
add_subdirectory(cmd)
add_subdirectory(storage)
add_subdirectory(suggestion)
add_subdirectory(workspace)
add_subdirectory(utils)

add_dependencies(${PROJECT_NAME} ai appmgr commandmgr storage suggestion workspace utils)
target_link_libraries(${PROJECT_NAME} PRIVATE commandmgr appmgr storage suggestion workspace utils)

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)

find_package(kubazip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE kubazip::kubazip)

target_link_directories(${PROJECT_NAME} PRIVATE ../build/lib)

target_include_directories(${PROJECT_NAME} PRIVATE ../include ../include/cmd ../include/appmgr ../include/suggestion ../include/workspace ../include/utils)