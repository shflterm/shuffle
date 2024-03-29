cmake_minimum_required(VERSION 3.22)
project(appmgr)

set(APPMGR_SRC appmgr.cpp downloader.cpp)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} STATIC ${APPMGR_SRC})

add_dependencies(${PROJECT_NAME} utils commandmgr)
target_link_libraries(${PROJECT_NAME} PUBLIC utils commandmgr)

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)

find_package(kubazip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE kubazip::kubazip)

target_link_directories(${PROJECT_NAME} PRIVATE ../build/lib)

target_include_directories(${PROJECT_NAME} PRIVATE ../../include)