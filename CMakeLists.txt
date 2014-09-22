project(com.ubuntu.filemanager C CXX)
cmake_minimum_required(VERSION 2.8.9)
find_program(INTLTOOL_MERGE intltool-merge)
find_program(INTLTOOL_EXTRACT intltool-extract)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-permissive -pedantic -Wall -Wextra -fPIC")

# Standard install paths
include(GNUInstallDirs)

option(INSTALL_TESTS "Install the tests on make install" on)
option(CLICK_MODE "Installs to a contained location" on)

set(APP_NAME filemanager)
set(DESKTOP_FILE "${PROJECT_NAME}.desktop")
set(LP_PROJECT ubuntu-filemanager-app)
set(ICON_FILE filemanager64.png)
set(AUTOPILOT_DIR ${APP_NAME})
set(EXEC "${APP_NAME}")
set(CONTENT_HUB_EXPORTER hub-exporter.json)
set(UBUNTU_MANIFEST_PATH "manifest.json.in" CACHE INTERNAL "Relative path to the manifest file")

if(CLICK_MODE)
    if(NOT DEFINED BZR_SOURCE)
        set(BZR_SOURCE "lp:${LP_PROJECT}")
        message("-- Setting BZR_SOURCE to ${BZR_SOURCE}")
    endif(NOT DEFINED BZR_SOURCE)
    # Find out the architecture for package building
    # to determine the plugin's installation path
    execute_process(
        COMMAND dpkg-architecture -qDEB_HOST_MULTIARCH
        OUTPUT_VARIABLE ARCH_TRIPLET
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(CMAKE_INSTALL_PREFIX /)
    set(CMAKE_INSTALL_BINDIR /)
    set(DATA_DIR /)
    set(ICON ${ICON_FILE})
    set(QT_IMPORTS_DIR "/lib/${ARCH_TRIPLET}")
    set(BIN_DIR /lib/${ARCH_TRIPLET}/bin)
    set(DESKTOP_DIR ${DATA_DIR})
    if(NOT BZR_REVNO)
        set(BZR_REVNO "latest")
    endif(NOT BZR_REVNO)

    #query the target architecture
    execute_process(
        COMMAND dpkg-architecture -qDEB_HOST_ARCH
        OUTPUT_VARIABLE CLICK_ARCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    configure_file(manifest.json.in ${CMAKE_CURRENT_BINARY_DIR}/manifest.json)

    # make the click files visible in qtcreator
    file(GLOB CLICK_FILES
         RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
         manifest.json.in *.apparmor)

    add_custom_target(com_ubuntu_calendar_CLICKFiles ALL SOURCES ${CLICK_FILES})

    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/manifest.json filemanager.apparmor ${CONTENT_HUB_EXPORTER} DESTINATION ${CMAKE_INSTALL_PREFIX})
else(CLICK_MODE)
    execute_process(
        COMMAND qmake -query QT_INSTALL_QML
        OUTPUT_VARIABLE QT_IMPORTS_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(DATA_DIR ${CMAKE_INSTALL_DATADIR}/${APP_NAME})
    set(ICON ${CMAKE_INSTALL_PREFIX}/${DATA_DIR}/${ICON_FILE})
    set(DESKTOP_DIR ${CMAKE_INSTALL_DATADIR}/applications)
endif(CLICK_MODE)

file(GLOB_RECURSE I18N_SRC_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/po
     *.qml *.js *.cpp *.h)
list(APPEND I18N_SRC_FILES ${DESKTOP_FILE}.in.in.h)
list(SORT I18N_SRC_FILES)

file(GLOB SRC_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     *.qml *.js *.png *.svg *.json)
install(FILES ${SRC_FILES}  DESTINATION ${DATA_DIR})

configure_file(${DESKTOP_FILE}.in.in ${DESKTOP_FILE}.in)

add_custom_target(${DESKTOP_FILE} ALL
    COMMENT "Merging translations into ${DESKTOP_FILE}..."
    COMMAND LC_ALL=C ${INTLTOOL_MERGE} -d -u ${CMAKE_SOURCE_DIR}/po ${DESKTOP_FILE}.in ${DESKTOP_FILE}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}
    DESTINATION ${DESKTOP_DIR})

# Tests
enable_testing()

add_subdirectory(po)
add_subdirectory(tests)
add_subdirectory(src)
