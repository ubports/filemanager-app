
TEMPLATE = app
CONFIG   += console
CONFIG   -= app_bundle

DEFINES +=  REGRESSION_TEST_QSAMBACLIENT

include (qsambaclient.pri)

include(test/testsuite.pri)

include (ubuntuFM/ubuntuFM_dependencies.pri)   

##main
SOURCES += $$PWD/test/main_testqsambaclient.cpp

OTHER_FILES += \
    TODO.txt




