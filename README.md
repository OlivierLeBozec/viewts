Tsview is a tool that enables you to analyse pts, pcr and dts in a ts MPEG2 stream.  
Qt and QChart are needed for this project.
Already compiled binaries (for windows and linux) are available in https://github.com/OlivierLeBozec/tstools/releases


![alt text](https://github.com/OlivierLeBozec/tstools/blob/master/screenshots/pcrjitter.png)

![alt text](https://github.com/OlivierLeBozec/tstools/blob/master/screenshots/pcrptsdts.png)

![alt text](https://github.com/OlivierLeBozec/tstools/blob/master/screenshots/ptspcr.png)

# compile tsview
To compile first import in qt_tstools/viewTS.pro in QtCreator.
Windows and Linux are supported.
Use Qt5.7 with mandatory QChart.

# compile CLI tools
Bunch of tools for MPEG-TS streams are also provided.
To compile first import in Qt top level CMakeLists.txt. It will generate two static libs, the cli tools and a binary for google tests.
Visual can also be used with proper CMake generator.
Windows and Linux are supported.

## tsmap
NAME
    ./tsmap - list mpeg ts content
    Version 1.0

SYNOPSIS
    ./tsmap FILE [-pidlist -pidmap]

DESCRIPTION
    List mpeg information about the FILE. Without using PSI-SI tables

    -pidlist
       list of the pid in the file

    -pidmap
       map of the pid in the file

## tscheck
NAME
    ./tscheck - check timestamp
    Version 1.0

SYNOPSIS
    ./tscheck FILE -pid <PID> [...]

DESCRIPTION
    Display specified timestamp operation

    -pidpcr <PID>
           set pcr pid

    -pidpts <PID>
           set pts pid

    -piddts <PID>
           set dts pid

Following commands are available only at least one PID is specified

    -dump
           dump timestamp

    -dur
           get duration of the stream

    -rate
           get bitrate of pid

    -delta
           diff between same consecutive timestamp for pcr, pts or dts

    -jitter
           display jitter for pcr

    -diff
           diff between 2 timestamps : pts - pcr, dts - pts or pts - dts

## tscut
   tscut - cut mpeg ts content
   Version 1.0

SYNOPSIS
   /home/olivier/tstools/tscut FILE [-split <nb packet>]

DESCRIPTION
   cut ts stream

    -split <nb packets>
          cut file every nb packets

## Tested examples:
    ./tsmap ./ts/audio.ts -pidlist -pidmap

    ./tscheck ./ts/audio.ts -pidpcr 305 -dur -rate
    ./tscheck ./ts/audio.ts -pidpcr 305 -dump
    ./tscheck ./ts/audio.ts -pidpts 305 -dump
    ./tscheck ./ts/audio.ts -pidpcr 305 -pidpts 305 -diff
    ./tscheck ./ts/audio.ts -pidpcr 305 -jitter
    ./tscheck ./ts/audio.ts -pidpcr 305 -delta

    ./tscheck ./ts/video.ts -pidpcr 601 -dump
    ./tscheck ./ts/video.ts -piddts 601 -dump
    ./tscheck ./ts/video.ts -pidpcr 601 -pidpts 601 -diff
    ./tscheck ./ts/video.ts -pidpcr 601 -pidpts 301 -diff
    ./tscheck ./ts/video.ts -piddts 601 -pidpts 601 -diff

# Linux env
Qt 5.12.0 (x86_64-little_endian-lp64 shared (dynamic) release build; by GCC 5.3.1 20160406 (Red Hat 5.3.1-6)) on "xcb" 

OS: CentOS Linux 7 (Core) [linux version 3.10.0-693.21.1.el7.x86_64]

From revision 7c17edcce2

Built on Nov 23 2018 01:26:40
