include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
)

SET(libnemofolderlistmodel_SRCS
    src/fmutil.cpp
    src/plugin.cpp
    src/dirmodel.cpp
    src/clipboard.cpp
    src/iorequest.cpp
    src/diriteminfo.cpp
    src/filecompare.cpp
    src/dirselection.cpp
    src/imageprovider.cpp
    src/ioworkerthread.cpp
    src/iorequestworker.cpp
    src/filesystemaction.cpp
    src/externalfswatcher.cpp
    src/trash/qtrashdir.cpp
)

add_library(libnemofolderlistmodel ${libnemofolderlistmodel_SRCS})
