contains($$list($$[QT_VERSION]),4.[6-9].*) {
    TEMPLATE = subdirs
} else {
    TEMPLATE = aux
}

the_includes.files += QMimeDatabase \
                      QMimeType \

unix:!symbian {
    maemo5 {
        the_includes.path = /opt/usr/include/qt5/QtMimeTypes
    } else {
        the_includes.path = /usr/include/qt5/QtMimeTypes
    }
    INSTALLS += the_includes
}
