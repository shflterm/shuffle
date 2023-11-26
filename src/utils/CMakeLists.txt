cmake_minimum_required(VERSION 3.22)
project(utils)

set(UTILS_SRC utils.cpp credit.cpp console.cpp i18n.cpp task.cpp shfljson.cpp)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} ${UTILS_SRC})

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

find_package(kubazip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE kubazip::kubazip)

target_link_directories(${PROJECT_NAME} PRIVATE ../build/lib)

target_include_directories(${PROJECT_NAME} PRIVATE ../../include)