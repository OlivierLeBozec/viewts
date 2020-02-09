![linux_compile](https://github.com/OlivierLeBozec/viewts/workflows/linux_compile/badge.svg)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/0fb4b49561494673ad88048db7b17943)](https://app.codacy.com/manual/OlivierLeBozec/viewts?utm_source=github.com&utm_medium=referral&utm_content=OlivierLeBozec/viewts&utm_campaign=Badge_Grade_Dashboard)

# viewts - Display timestamps and related feature

viewts is a tool that enables you to analyse pts, pcr and dts in a ts MPEG2 stream.

Qt and QChart are needed for this project.

    Qt Creator 4.11.0-rc1 (4.10.84)
    Basé sur Qt 5.14.0 (GCC 5.3.1 20160406 (Red Hat 5.3.1-6), 64 bit)

Already compiled binaries (for windows and linux) are available in <https://github.com/OlivierLeBozec/tstools/releases>

Some binary tools can be used in command line. They can be used for tests automatisation.

## Feature

Display :
* PCR, PTS, DTS

* continuity counter error

* Random access point

Compute :

  - Bitrate

  - PCR jitter

  - Diff between timestamps

  - ES buffer level, basically the VBV

Other features :

  - X axis can be set to time or packet number

  - Save data into file

  - command line interface

  - ts file drag and drop

  - zoom on data

## Screenshots

![alt text](https://github.com/OlivierLeBozec/tstools/blob/master/screenshots/pcrjitter.png)

![alt text](https://github.com/OlivierLeBozec/tstools/blob/master/screenshots/pcrptsdts.png)

![alt text](https://github.com/OlivierLeBozec/tstools/blob/master/screenshots/ptspcr.png)

## compile viewts
To compile first import in qt_tstools/viewTS.pro in QtCreator.

Windows and Linux are supported.

Use Qt5.7 with mandatory QChart.

## compile CLI tools
Bunch of tools for MPEG-TS streams are also provided.

To compile first import in Qt top level CMakeLists.txt.

It will generate two static libs, the cli tools and a binary for google tests.

Visual can also be used with proper CMake generator.

Windows and Linux are supported.

## How to Contribute

We welcome contributions of all kinds, including bug fixes, new features,
documentation and translations. By contributing, you agree to release
your contributions under the terms of the license.

Contribute by following the typical
[GitHub workflow](https://guides.github.com/introduction/flow/index.html)
for pull requests. Fork the repository and make changes on a new named
branch. Create pull requests against the `master` branch. Follow the
[seven guidelines](https://chris.beams.io/posts/git-commit/) to writing a
great commit message.

### tsmap
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

### tscheck
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

### tscut
    NAME
       tscut - cut mpeg ts content
       Version 1.1

    SYNOPSIS
       ./tscut FILE [-split <nb packet>] [-divide <nb chunck>]

    DESCRIPTION
       cut ts stream

    -split <nb packets>
          cut file every nb packets
    -divide <nb chunks>
          cut file in nb chunks

### Tested examples
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
