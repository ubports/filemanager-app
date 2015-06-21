QT       *= testlib
TEMPLATE  = app
CONFIG   *= test

## this suite can be included in any Test Case main file
SOURCES += $$PWD/testqsambasuite.cpp
HEADERS += $$PWD/testqsambasuite.h

INCLUDEPATH += $$PWD
