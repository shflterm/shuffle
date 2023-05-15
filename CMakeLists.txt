cmake_minimum_required(VERSION 3.18)

project(shuffle)

add_subdirectory(${CMAKE_SOURCE_DIR}/commands)

set(SOURCES src/suggestion.cpp src/utils/utils.cpp src/console.cpp src/i18n.cpp src/commandmgr.cpp src/sapp/sapp.cpp src/workspace.cpp src/utils/cmdexecutor.cpp include/libsapp.h src/sapp/downloader.cpp)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${PROJECT_NAME} ${SOURCES} src/main.cpp include/basic_commands.h)
add_library(${PROJECT_NAME}_lib STATIC ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)
target_compile_definitions(${PROJECT_NAME}_lib PRIVATE _HAS_STD_BYTE=0)

find_package(jsoncpp CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME}  JsonCpp::JsonCpp)
target_link_libraries(${PROJECT_NAME}_lib  JsonCpp::JsonCpp)

find_package(CURL CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} CURL::libcurl)
target_link_libraries(${PROJECT_NAME}_lib CURL::libcurl)

find_package(kubazip CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} kubazip::kubazip)
target_link_libraries(${PROJECT_NAME}_lib kubazip::kubazip)

target_link_directories(${PROJECT_NAME} PRIVATE build/lib)
target_link_directories(${PROJECT_NAME}_lib PRIVATE build/lib)

target_include_directories(${PROJECT_NAME} PRIVATE include)
target_include_directories(${PROJECT_NAME}_lib PRIVATE include)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_features(${PROJECT_NAME}_lib PRIVATE cxx_std_17)