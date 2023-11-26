cmake_minimum_required(VERSION 3.22)
project(commandmgr)

set(COMMAND_SRC commandmgr.cpp job.cpp cmdparser.cpp builtincmd.cpp)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} ${COMMAND_SRC})

add_dependencies(${PROJECT_NAME} utils)
target_link_libraries(${PROJECT_NAME} PUBLIC utils)

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE JsonCpp::JsonCpp)

find_package(Lua CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE ${LUA_LIBRARIES})

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE CURL::libcurl)

find_package(kubazip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE kubazip::kubazip)

target_link_directories(${PROJECT_NAME} PRIVATE ../build/lib)

target_include_directories(${PROJECT_NAME} PRIVATE ../../include)