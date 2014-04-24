include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(folderlistmodel_SRCS
    clipboard.cpp
    clipboard.h
    diritemabstractlistmodel.h
    diriteminfo.cpp
    diriteminfo.h
    dirmodel.cpp
    dirmodel.h
    dirselection.cpp
    dirselection.h
    externalfswatcher.cpp
    externalfswatcher.h
    filecompare.cpp
    filecompare.h
    filesystemaction.cpp
    filesystemaction.h
    fmutil.cpp
    fmutil.h
    imageprovider.cpp
    imageprovider.h
    iorequest.cpp
    iorequest.h
    iorequestworker.cpp
    iorequestworker.h
    ioworkerthread.cpp
    ioworkerthread.h
    plugin.cpp
    plugin.h
    trash/qtrashdir.cpp
    trash/qtrashdir.h
)

add_library(nemofolderlistmodel MODULE
    ${folderlistmodel_SRCS}
)

qt5_use_modules(nemofolderlistmodel Gui Qml Quick Widgets)

# Copy qmldir file to build dir for running in QtCreator
if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    add_custom_target(folderlistmodel-qmldir ALL
        COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/qmldir ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS ${QMLFILES}
    )
endif(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")

# Install plugin file
install(TARGETS nemofolderlistmodel DESTINATION ${QT_IMPORTS_DIR}/org/nemomobile/folderlistmodel/)
install(FILES qmldir DESTINATION ${QT_IMPORTS_DIR}/org/nemomobile/folderlistmodel/)

