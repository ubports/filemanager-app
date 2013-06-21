SOURCES += $$PWD/dirmodel.cpp \
           $$PWD/iorequest.cpp \
           $$PWD/iorequestworker.cpp \
           $$PWD/ioworkerthread.cpp \
           $$PWD/filesystemaction.cpp \
           $$PWD/imageprovider.cpp \




HEADERS += $$PWD/dirmodel.h \
           $$PWD/iorequest.h \
           $$PWD/iorequestworker.h \
           $$PWD/ioworkerthread.h \
           $$PWD/filesystemaction.h \
           $$PWD/imageprovider.h \



INCLUDEPATH  += $$PWD 

greaterThan(QT_MAJOR_VERSION, 4) {
   QT += qml
}
else {
    QT += declarative    
}

