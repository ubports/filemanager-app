#-------------------------------------------------
#
# Project created by QtCreator 2015-09-05T17:37:49
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_placesmodeltest
CONFIG   += console
CONFIG   += testcase
CONFIG   -= app_bundle

TEMPLATE = app

DEFINES += REGRESSION_TEST_PLACES_MODEL

DEFINES += SRCDIR=\\\"$$PWD/\\\"

QMAKE_CXXFLAGS += -std=c++11


SOURCES += placesmodeltest.cpp \
    ../placesmodel/placesmodel.cpp \
    ../placesmodel/qmtabparser.cpp


HEADERS += \    
    ../placesmodel/placesmodel.h \
    ../placesmodel/qmtabparser.h


INCLUDEPATH += ../placesmodel
