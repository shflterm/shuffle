cmake_minimum_required(VERSION 3.22)
project(suggestion)

set(SUGGESTION_SRC suggestion.cpp)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} ${SUGGESTION_SRC})

add_dependencies(${PROJECT_NAME} term_cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)

target_link_libraries(${PROJECT_NAME} term_cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ../termcpp/include)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} JsonCpp::JsonCpp)

find_package(Lua CONFIG REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${LUA_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${LUA_LIBRARIES})

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} CURL::libcurl)

find_package(kubazip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} kubazip::kubazip)

target_link_directories(${PROJECT_NAME} PRIVATE ../build/lib)

target_include_directories(${PROJECT_NAME} PRIVATE ../../include ../../include/cmd ../../include/sapp ../../include/suggestion ../../include/workspace ../../include/utils)