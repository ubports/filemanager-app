
SOURCES +=   $$PWD/src/smbutil.cpp 
             


HEADERS +=   $$PWD/src/smbutil.h 
                     

QT        *= core network             

CONFIG    *= link_pkgconfig
PKGCONFIG *= smbclient

INCLUDEPATH += $$PWD/src
