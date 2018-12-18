#-------------------------------------------------
#
# Project created by QtCreator 2016-11-21T21:27:33
#
#-------------------------------------------------

QT      += core gui charts widgets
CONFIG  += static

TARGET = viewTS
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    chart.cpp \
    chartview.cpp \
    thread.cpp

HEADERS  += mainwindow.h \
    chart.h \
    chartview.h \
    thread.h

RC_FILE = viewTS.rc

win32 {
  DESTDIR = $$PWD/../bin/windows/
  INCLUDEPATH += $$PWD/../bin/windows
  DEPENDPATH += $$PWD/../bin/windows
}

linux {
  DESTDIR = $$PWD/../bin/linux/
  INCLUDEPATH += $$PWD/../bin/linux
  DEPENDPATH += $$PWD/../bin/linux
  QMAKE_CXXFLAGS += -fpermissive
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/windows/ -ltstoolcheck -ltstoolmap
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/windows/ -ltstoolcheck -ltstoolmap

linux:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/linux/ -ltstoolcheck -ltstoolmap
else:linux:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/linux/ -ltstoolcheck -ltstoolmap


win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/windows/libtstoolcheck.a $$PWD/../bin/windows/libtstoolmap.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/windows/libtstoolcheck.a $$PWD/../bin/windows/libtstoolmap.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/windows/tstoolcheck.lib $$PWD/../bin/windows/tstoolmap.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/windows/tstoolcheck.lib $$PWD/../bin/windows/tstoolmap.lib

linux-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/linux/libtstoolcheck.a $$PWD/../bin/linux/libtstoolmap.a
else:linux-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/linux/libtstoolcheck.a $$PWD/../bin/linux/libtstoolmap.a
else:linux:!linux-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/linux/tstoolcheck.so $$PWD/../bin/linux/tstoolmap.so
else:linux:!linux-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/linux/tstoolcheck.so $$PWD/../bin/linux/tstoolmap.so
