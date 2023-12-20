cmake_minimum_required(VERSION 3.22)
project(workspace)

set(WORKSPACE_SRC workspace.cpp snippetmgr.cpp crashreport.cpp)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} STATIC ${WORKSPACE_SRC})

add_dependencies(${PROJECT_NAME} ai utils commandmgr suggestion)
target_link_libraries(${PROJECT_NAME} PUBLIC ai utils commandmgr suggestion)

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

find_package(kubazip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE kubazip::kubazip)

find_package(restclient-cpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE restclient-cpp)

target_link_directories(${PROJECT_NAME} PRIVATE ../build/lib)

target_include_directories(${PROJECT_NAME} PRIVATE ../../include)