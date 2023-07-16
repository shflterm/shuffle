cmake_minimum_required(VERSION 3.18)
project(shuffle)

set(SOURCES suggestion.cpp utils/utils.cpp console.cpp i18n.cpp cmd/commandmgr.cpp sapp/sapp.cpp workspace.cpp
        ../include/libsapp.h sapp/downloader.cpp utils/lua/luaapi.cpp utils/crashreport.cpp utils/credit.cpp cmd/builtincmd.cpp snippetmgr.cpp cmd/parsedcmd.cpp cmd/cmdparser.cpp)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_executable(${PROJECT_NAME} ${SOURCES} main.cpp)

target_compile_definitions(${PROJECT_NAME} PRIVATE _HAS_STD_BYTE=0)

target_link_libraries(${PROJECT_NAME} term_cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ../term.cpp/include)

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

target_include_directories(${PROJECT_NAME} PRIVATE ../include)
