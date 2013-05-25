SOURCES += $$PWD/src/mimetypes/inqt5/qstandardpaths.cpp
SOURCES += $$PWD/src/mimetypes/inqt5/qstandardpaths_unix.cpp
SOURCES += $$PWD/src/mimetypes/qmimedatabase.cpp
SOURCES += $$PWD/src/mimetypes/qmimeglobpattern.cpp
SOURCES += $$PWD/src/mimetypes/qmimemagicrule.cpp
SOURCES += $$PWD/src/mimetypes/qmimemagicrulematcher.cpp
SOURCES += $$PWD/src/mimetypes/qmimeprovider.cpp
SOURCES += $$PWD/src/mimetypes/qmimetype.cpp
SOURCES += $$PWD/src/mimetypes/qmimetypeparser.cpp


HEADERS += $$PWD/src/mimetypes/inqt5/qstandardpaths.h
HEADERS += $$PWD/src/mimetypes/qmime_global.h
HEADERS += $$PWD/src/mimetypes/qmimedatabase.h
HEADERS += $$PWD/src/mimetypes/qmimedatabase_p.h
HEADERS += $$PWD/src/mimetypes/qmimeglobpattern_p.h
HEADERS += $$PWD/src/mimetypes/qmimemagicrule_p.h
HEADERS += $$PWD/src/mimetypes/qmimemagicrulematcher_p.h
HEADERS += $$PWD/src/mimetypes/qmimeprovider_p.h
HEADERS += $$PWD/src/mimetypes/qmimetype.h
HEADERS += $$PWD/src/mimetypes/qmimetype_p.h
HEADERS += $$PWD/src/mimetypes/qmimetypeparser_p.h

INCLUDEPATH +=  $$PWD/include/QtMimeTypes $$PWD/src/mimetypes $$PWD/src/mimetypes/inqt5

RESOURCES +=  $$PWD/src/mimetypes/mimetypes.qrc
