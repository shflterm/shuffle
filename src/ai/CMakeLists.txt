cmake_minimum_required(VERSION 3.22)
project(ai)

set(AI_SRC shflai.cpp)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} STATIC ${AI_SRC})

add_dependencies(${PROJECT_NAME} appmgr)
target_link_libraries(${PROJECT_NAME} PUBLIC appmgr)

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)

find_package(kubazip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE kubazip::kubazip)

target_link_directories(${PROJECT_NAME} PRIVATE ../build/lib)

configure_file(gemini.h.in ${CMAKE_BINARY_DIR}/generated/gemini.h)
include_directories(${CMAKE_BINARY_DIR}/generated/)

target_include_directories(${PROJECT_NAME} PRIVATE ../../include)