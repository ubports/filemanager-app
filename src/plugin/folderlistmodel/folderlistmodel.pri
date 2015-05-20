SOURCES += $$PWD/dirmodel.cpp \
           $$PWD/iorequest.cpp \
           $$PWD/iorequestworker.cpp \
           $$PWD/ioworkerthread.cpp \
           $$PWD/filesystemaction.cpp \
           $$PWD/filecompare.cpp \
           $$PWD/externalfswatcher.cpp \
           $$PWD/clipboard.cpp \
           $$PWD/fmutil.cpp \
           $$PWD/dirselection.cpp \
           $$PWD/diriteminfo.cpp \
           $$PWD/urliteminfo.cpp \
           $$PWD/location.cpp \
           $$PWD/locationsfactory.cpp \                    
           $$PWD/locationurl.cpp \             
           $$PWD/locationitemdiriterator.cpp \
           $$PWD/cleanurl.cpp

HEADERS += $$PWD/dirmodel.h \
           $$PWD/iorequest.h \
           $$PWD/iorequestworker.h \
           $$PWD/ioworkerthread.h \
           $$PWD/filesystemaction.h \
           $$PWD/filecompare.h \
           $$PWD/externalfswatcher.h \
           $$PWD/clipboard.h \
           $$PWD/fmutil.h  \
           $$PWD/dirselection.h \          
           $$PWD/diritemabstractlistmodel.h \
           $$PWD/diriteminfo.h \
           $$PWD/urliteminfo.h \           
           $$PWD/location.h \
           $$PWD/locationsfactory.h \                   
           $$PWD/locationurl.h \          
           $$PWD/locationitemdiriterator.h \
           $$PWD/cleanurl.h

SOURCES +=  $$PWD/disk/disklocation.cpp
HEADERS +=  $$PWD/disk/disklocation.h

SOURCES +=  $$PWD/trash/qtrashdir.cpp $$PWD/trash/trashiteminfo.cpp  \
            $$PWD/trash/qtrashutilinfo.cpp $$PWD/trash/trashlocation.cpp
HEADERS +=  $$PWD/trash/qtrashdir.h       $$PWD/trash/trashiteminfo.h    \
            $$PWD/trash/qtrashutilinfo.h  $$PWD/trash/trashlocation.h

SOURCES +=  $$PWD/smb/smblocation.cpp $$PWD/smb/smblocationauthentication.cpp $$PWD/smb/smblistworker.cpp
HEADERS +=  $$PWD/smb/smblocation.h $$PWD/smb/smblocationauthentication.h $$PWD/smb/smblistworker.h

SOURCES +=  $$PWD/net/netutil.cpp $$PWD/net/netauthenticationdata.cpp
HEADERS +=  $$PWD/net/netutil.h $$PWD/net/netauthenticationdata.h

INCLUDEPATH  += $$PWD $$PWD/trash $$PWD/disk $$PWD/smb $$PWD/net

include ($$PWD/smb/qsambaclient/qsambaclient.pri)

greaterThan(QT_MAJOR_VERSION, 4) {
   QT += qml
}
else {
    QT += declarative    
}


!contains (DEFINES, DO_NOT_USE_TAG_LIB) {
   LIBS += -ltag
   SOURCES += $$PWD/imageprovider.cpp 
   HEADERS += $$PWD/imageprovider.h 
}
