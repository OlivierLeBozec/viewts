#-------------------------------------------------
#
# Project created by QtCreator 2016-11-21T21:27:33
#
#-------------------------------------------------

QT += core gui charts widgets

TARGET = viewTS
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    chart.cpp \
    chartview.cpp \
    workers.cpp \
    ../src/timestamp.cpp \
    ../src/pes.cpp \
    ../src/packet.cpp \
    ../src/pidmap.cpp

HEADERS  += mainwindow.h \
    chart.h \
    chartview.h \
    workers.h \
    ../src/timestamp.h \
    ../src/pes.h \
    ../src/packet.h \
    ../src/pidmap.h


RC_FILE = viewTS.rc

win32 {
  DESTDIR = $$PWD/../bin/windows/
  INCLUDEPATH += $$PWD/../bin/windows
  DEPENDPATH += $$PWD/../bin/windows
  CONFIG  += static
}

linux {
  DESTDIR = $$PWD/../bin/linux/
  INCLUDEPATH += $$PWD/../bin/linux
  DEPENDPATH += $$PWD/../bin/linux
}
