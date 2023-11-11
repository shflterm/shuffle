cmake_minimum_required(VERSION 3.22)
project(unit_test)

#add_executable(test_suggest suggest.cpp)
#target_link_libraries(test_suggest commandmgr appmgr suggestion utils)
#add_dependencies(test_suggest commandmgr appmgr suggestion utils)
#
#add_executable(test_history history.cpp)
#target_link_libraries(test_history commandmgr appmgr suggestion utils)
#add_dependencies(test_history commandmgr appmgr suggestion utils)

include_directories(../include/cmd ../include/app ../include/suggestion ../include/workspace ../include/utils)

target_compile_features(test_suggest PRIVATE cxx_std_17)