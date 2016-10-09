cmake_minimum_required(VERSION 2.8)

project(tscheck)

#Source files
set(SOURCE_FILES
    pes.cpp
    timestamp.cpp
    packet.cpp
    tscheck.cpp
)
source_group(sources FILES ${SOURCE_FILES})

#Header files
set(HEADER_FILES
    pes.h
    timestamp.h
    packet.h
)
source_group(headers FILES ${HEADER_FILES})

#Binary
add_executable(tscheck ${SOURCE_FILES} ${HEADER_FILES})

#----------------------------------------------------------------
project(tsmap)

#Source files
set(SOURCE_FILES
    pidmap.cpp
    packet.cpp
    pes.cpp
    tsmap.cpp
)
source_group(sources FILES ${SOURCE_FILES})

#Header files
set(HEADER_FILES
    pidmap.h
    packet.h
    pes.h
)
source_group(headers FILES ${HEADER_FILES})

#Binary
add_executable(tsmap ${SOURCE_FILES} ${HEADER_FILES})
