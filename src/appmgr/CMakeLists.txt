cmake_minimum_required(VERSION 3.22)
project(appmgr)

set(APPMGR_SRC appmgr.cpp downloader.cpp)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} ${APPMGR_SRC})

add_dependencies(${PROJECT_NAME} utils commandmgr workspace)
target_link_libraries(${PROJECT_NAME} PUBLIC utils commandmgr workspace)

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

find_package(Python COMPONENTS Interpreter Development)
if (Python_FOUND)
    if (Python_VERSION VERSION_GREATER_EQUAL "3.11" AND Python_VERSION VERSION_LESS "3.12")
        include_directories(${Python_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} PRIVATE ${Python_LIBRARIES})
    else()
        message(FATAL_ERROR "Found Python version ${Python_VERSION}, but version 3.11 is required.")
    endif()
else()
    message(FATAL_ERROR "Python not found. Please check your Python installation.")
endif()

find_package(pybind11 CONFIG)

target_link_libraries(${PROJECT_NAME} PRIVATE pybind11::pybind11)
set_target_properties(${PROJECT_NAME} PROPERTIES
        INTERPROCEDURAL_OPTIMIZATION ON
        CXX_VISIBILITY_PRESET ON
        VISIBILITY_INLINES_HIDDEN ON
)

target_link_directories(${PROJECT_NAME} PRIVATE ../build/lib)

target_include_directories(${PROJECT_NAME} PRIVATE ../../include ../../include/cmd ../../include/app ../../include/suggestion ../../include/workspace ../../include/utils)