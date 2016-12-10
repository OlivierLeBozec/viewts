#-------------------------------------------------
#
# Project created by QtCreator 2016-11-21T21:27:33
#
#-------------------------------------------------

QT      += core gui charts widgets
CONFIG  += static

TARGET = qt_tscheck
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    chart.cpp \
    chartview.cpp

HEADERS  += mainwindow.h \
    chart.h \
    chartview.h

FORMS    += mainwindow.ui

win32 {
  DESTDIR = $$PWD/../bin/windows/
}

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../bin/windows/ -ltstoolcheck -ltstoolmap
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../bin/windows/ -ltstoolcheck -ltstoolmap

INCLUDEPATH += $$PWD/../bin/windows
DEPENDPATH += $$PWD/../bin/windows

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/windows/libtstoolcheck.a $$PWD/../bin/windows/libtstoolmap.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/windows/libtstoolcheck.a $$PWD/../bin/windows/libtstoolmap.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../bin/windows/tstoolcheck.lib $$PWD/../bin/windows/tstoolmap.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../bin/windows/tstoolcheck.lib $$PWD/../bin/windows/tstoolmap.lib
