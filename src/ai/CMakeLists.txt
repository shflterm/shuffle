cmake_minimum_required(VERSION 3.22)
project(ai)

set(AI_SRC genllama.cpp)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} ${AI_SRC})

add_dependencies(${PROJECT_NAME} utils commandmgr workspace)
target_link_libraries(${PROJECT_NAME} PUBLIC utils commandmgr workspace)

install(TARGETS ${PROJECT_NAME} RUNTIME)
target_link_libraries(${PROJECT_NAME} PRIVATE common llama ${CMAKE_THREAD_LIBS_INIT})
target_include_directories(${PROJECT_NAME} PRIVATE ../../llama.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

find_package(kubazip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE kubazip::kubazip)

target_link_directories(${PROJECT_NAME} PRIVATE ../build/lib)

target_include_directories(${PROJECT_NAME} PRIVATE ../../include)