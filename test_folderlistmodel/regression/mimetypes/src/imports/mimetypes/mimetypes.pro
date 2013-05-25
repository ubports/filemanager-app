include(../../../mimetypes-nolibs.pri)
LIBS += -L$$OUT_PWD/../../../src/mimetypes -lQtMimeTypes

CONFIG   += qt plugin
TEMPLATE = lib
TARGET   = declarative_mimetypes

# QtCore/qlist.h uses /usr/include/limits.h which uses does not compile with -pedantic.
# QtDeclarative/qdeclarativeprivate.h will not compile with -pedantic.
#MAKE_CXXFLAGS += -W -Wall -Wextra -Werror -pedantic -Wshadow -Wno-long-long -Wnon-virtual-dtor
QMAKE_CXXFLAGS += -W -Wall -Wextra -Werror           -Wshadow -Wno-long-long -Wnon-virtual-dtor


CONFIG += depend_includepath

QT += declarative quick


SOURCES += mimetypes.cpp

# No headers


SOURCES += qdeclarativemimetype.cpp \
           qdeclarativemimedatabase.cpp

HEADERS += qdeclarativemimetype_p.h \
           qdeclarativemimedatabase_p.h


qmldir.files += $$PWD/qmldir plugins.qmltypes


unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib/qt5/imports/QtMimeTypes
        qmldir.path = /opt/usr/lib/qt5/imports/QtMimeTypes
    } else {
        target.path = /usr/lib/qt5/imports/QtMimeTypes
        qmldir.path = /usr/lib/qt5/imports/QtMimeTypes
    }
    INSTALLS += qmldir target
}
