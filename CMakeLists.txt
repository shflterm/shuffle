cmake_minimum_required(VERSION 3.18)

project(shuffle)

include(CMakePrintHelpers)

set(SOURCES src/suggestion.cpp src/utils/utils.cpp src/console.cpp src/i18n.cpp src/commandmgr.cpp src/sapp/sapp.cpp src/workspace.cpp src/utils/cmdexecutor.cpp)

add_executable(${PROJECT_NAME} ${SOURCES} src/main.cpp include/basic_commands.h)

find_package(jsoncpp REQUIRED)
include_directories(JsonCpp::JsonCpp)
target_link_libraries(${PROJECT_NAME} JsonCpp::JsonCpp)

target_link_directories(${PROJECT_NAME} PRIVATE build/lib)
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
