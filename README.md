# tstools
Tools for MPEG-TS streams.
To compile use cmake for windows or linux.

## tsmap
NAME
    tsmap - list mpeg ts content

SYNOPSIS
    tsmap FILE [-pidlist -pidmap]

DESCRIPTION
    List mpeg information about the FILE. Without using PSI-SI tables.

    -pidlist
          list of the pid in the file

    -pidmap
          map of the pid in the file

## tscheck
NAME
   tscheck - check timestamp

SYNOPSIS
   tscheck FILE -pid <PID> [...]

DESCRIPTION
   Display specified timestamp

    -pid <PID>
          pid to check for

   Following commands are available only if the PID is specified

    -dur
          get duration of the stream

    -rate
          get  bitrate of the stream

    -delta
          diff between consecutive pcr

    -pcr
          display pcr

    -pts
          display pts

    -dts
          display dts

    -jitter
          display pcr jitter

    -lvl
        display input buffer level

## Tested examples:
    ./tsmap /home/olivier/Dropbox/Code/Qt/tstools/ts/Jitter/6.ts -pidlist -pidmap

    ./tscheck /home/olivier/Dropbox/Code/Qt/tstools/ts/Jitter/6.ts -pid 305 -dur -rate
    ./tscheck /home/olivier/Dropbox/Code/Qt/tstools/ts/Jitter/6.ts -pid 305 -pcr
    ./tscheck /home/olivier/Dropbox/Code/Qt/tstools/ts/Jitter/6.ts -pid 305 -pts
    ./tscheck /home/olivier/Dropbox/Code/Qt/tstools/ts/Jitter/6.ts -pid 305 -lvl
    ./tscheck /home/olivier/Dropbox/Code/Qt/tstools/ts/Jitter/6.ts -pid 305 -jitter
    ./tscheck ~/Bureau/MPTS_Txt-Subtitle.ts -pid 3502 -pcr
