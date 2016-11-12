project(tstools CXX)
cmake_minimum_required(VERSION 2.8)

add_subdirectory(src)
if (NOT WIN32)
    add_subdirectory(test)
endif()
