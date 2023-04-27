cmake_minimum_required(VERSION 3.18)

# enable compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

project(shuffle)

# Find SDL2 library
#find_package(SDL2 REQUIRED)

# Add the include directories for the SDL2 library
#include_directories(${SDL2_INCLUDE_DIRS})

# Сюда пишем .cpp в source/
set(SOURCES src/main.cpp)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Create symlink to compile_commands.json for clangd
if(UNIX)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E create_symlink ${CMAKE_CURRENT_BINARY_DIR}/compile_commands.json ${CMAKE_SOURCE_DIR}/compile_commands.json)
endif (UNIX)

# Link the SDL2 library
target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
