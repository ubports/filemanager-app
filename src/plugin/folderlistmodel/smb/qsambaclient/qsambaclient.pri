
SOURCES +=   $$PWD/src/smbutil.cpp \
             $$PWD/src/smbusershare.cpp \
             $$PWD/src/smbiteminfo.cpp \
             $$PWD/src/smbplaces.cpp \
             $$PWD/src/smbobject.cpp \
             $$PWD/src/smblocationdiriterator.cpp


HEADERS +=   $$PWD/src/smbutil.h \
             $$PWD/src/smbusershare.h \
             $$PWD/src/smbiteminfo.h \
             $$PWD/src/smbplaces.h \
             $$PWD/src/smbobject.h \
             $$PWD/src/smblocationdiriterator.h
          
QT          *= core network             

CONFIG    *= link_pkgconfig
PKGCONFIG *= smbclient

INCLUDEPATH += $$PWD/src
