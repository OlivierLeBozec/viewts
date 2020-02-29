project(tstools CXX)
cmake_minimum_required(VERSION 2.8)

#---------------------------------------------------------------- target tstoolcheck lib
#source lib files
set(SOURCE_LIB_FILES
    pes.cpp
    timestamp.cpp
    packet.cpp
)

#header lib files
set(HEADER_LIB_FILES
    pes.h
    timestamp.h
    packet.h
)

# generate library
add_library(tstoolcheck ${SOURCE_LIB_FILES} ${HEADER_LIB_FILES})

#---------------------------------------------------------------- target tscheck
#source files
set(SOURCE_FILES
    tscheck.cpp
)

#generate binary
add_executable(tscheck ${SOURCE_FILES})

#build dependencies
target_link_libraries(tscheck tstoolcheck)

#---------------------------------------------------------------- target tstoolmap lib
#source lib files
set(SOURCE_LIB_FILES
    pidmap.cpp
    packet.cpp
    pes.cpp
)

#header lib files
set(HEADER_LIB_FILES
    pidmap.h
    packet.h
    pes.h
)

# generate library
add_library(tstoolmap ${SOURCE_LIB_FILES} ${HEADER_LIB_FILES})

#---------------------------------------------------------------- target tsmap
#source files
set(SOURCE_FILES
    tsmap.cpp
)

add_executable(tsmap ${SOURCE_FILES} ${HEADER_FILES})

#build dependencies
target_link_libraries(tsmap tstoolmap)
