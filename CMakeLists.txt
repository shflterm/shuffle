cmake_minimum_required(VERSION 3.18)

project(shuffle)

set(SOURCES src/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_directories(${PROJECT_NAME} PRIVATE lib)
target_link_libraries(${PROJECT_NAME} PRIVATE libjsoncpp.so)
target_include_directories(${PROJECT_NAME} PRIVATE include)

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
