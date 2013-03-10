#-------------------------------------------------
#
# Project created by QtCreator 2013-03-09T12:30:11
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app

DEFINES += REGRESSION_TEST_FOLDERLISTMODEL

SOURCES += main.cpp\
        simplelist.cpp

HEADERS  += simplelist.h

FORMS    += simplelist.ui

include (../../folderlistmodel/folderlistmodel.pri)



