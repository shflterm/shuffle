cmake_minimum_required(VERSION 3.18)

project(shuffle)

set(SOURCES src/executor.cpp src/suggestion.cpp src/utils/utils.cpp src/console.cpp src/i18n.cpp src/commandmgr.cpp src/sapp/sapp.cpp include/utils/cmdexecutor.h include/basic_commands.h src/utils/cmdexecutor.cpp)

add_subdirectory(jsoncpp)

add_executable(${PROJECT_NAME} ${SOURCES} src/main.cpp)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_link_directories(${PROJECT_NAME} PRIVATE lib/Debug)
elseif (CMAKE_BUILD_TYPE STREQUAL "Release")
    target_link_directories(${PROJECT_NAME} PRIVATE lib/Release)
endif()

if (WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE jsoncpp.lib)
elseif (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE libjsoncpp.a)
endif ()

target_include_directories(${PROJECT_NAME} PRIVATE include)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
