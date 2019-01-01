project(tstools CXX)
cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 11)

#exe path
if (NOT WIN32)
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/linux)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/linux)
else()
    set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/windows)
    set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin/windows)
endif()

add_subdirectory(src)
add_subdirectory(test)
