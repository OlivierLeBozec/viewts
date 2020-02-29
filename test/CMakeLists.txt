project(tstools CXX)
cmake_minimum_required(VERSION 2.8)

add_subdirectory(googletest-release-1.8.1)

#---------------------------------------------------------------- target test
#source files
set(SOURCE_FILES
    test.cpp
)

include_directories(googletest-release-1.8.1/googletest/include)
add_executable(tstool_tests ${SOURCE_FILES})
add_dependencies(tstool_tests gtest gtest_main tstoolcheck)
target_link_libraries(tstool_tests gtest gtest_main tstoolcheck tstoolmap)
