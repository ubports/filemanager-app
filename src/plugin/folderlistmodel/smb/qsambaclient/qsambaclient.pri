
SOURCES +=   $$PWD/src/smbutil.cpp \
             $$PWD/src/smbusershare.cpp \
             $$PWD/src/smbiteminfo.cpp \
             $$PWD/src/smbplaces.cpp \
             $$PWD/src/smbobject.cpp \
             $$PWD/src/smblocationdiriterator.cpp \
             $$PWD/src/smblocationitemfile.cpp \
             $$PWD/src/smblocationitemdir.cpp


HEADERS +=   $$PWD/src/smbutil.h \
             $$PWD/src/smbusershare.h \
             $$PWD/src/smbiteminfo.h \
             $$PWD/src/smbplaces.h \
             $$PWD/src/smbobject.h \
             $$PWD/src/smblocationdiriterator.h \
             $$PWD/src/smblocationitemfile.h \
             $$PWD/src/smblocationitemdir.h

          
QT          *= core network             

CONFIG    *= link_pkgconfig
PKGCONFIG *= smbclient

DEFINES *= _LARGEFILE_SOURCE _FILE_OFFSET_BITS=64

INCLUDEPATH += $$PWD/src
