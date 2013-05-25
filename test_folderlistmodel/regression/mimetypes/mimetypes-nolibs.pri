INCLUDEPATH += $$PWD/include/QtMimeTypes
INCLUDEPATH += $$PWD/src/mimetypes/inqt5
INCLUDEPATH += $$PWD/src/mimetypes $$PWD/src/imports/mimetypes

mac|darwin: {
    QMAKE_CXXFLAGS += -ansi
} else:false {
    QMAKE_CXXFLAGS += -ansi -Wc++0x-compat
} else {
    QMAKE_CXXFLAGS += -std=c++0x
}
