project(ubuntu-filemanager-app C CXX)
cmake_minimum_required(VERSION 2.8.9)
find_program(INTLTOOL_MERGE intltool-merge)
find_program(INTLTOOL_EXTRACT intltool-extract)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fno-permissive -pedantic -Wall -Wextra -fPIC")

# Standard install paths
include(GNUInstallDirs)

option(INSTALL_TESTS "Install the tests on make install" on)

set(APP_NAME ubuntu-filemanager-app)
set(DESKTOP_FILE "${PROJECT_NAME}.desktop")
set(LP_PROJECT ubuntu-filemanager-app)
set(ICON_FILE filemanager64.png)
set(AUTOPILOT_DIR ${APP_NAME})
set(EXEC "${APP_NAME}")
set(CONTENT_HUB_JSON content-hub.json)

# global FILE definitions for stat system call, it works for any sub module
add_definitions(-D_LARGEFILE_SOURCE)
add_definitions(-D_FILE_OFFSET_BITS=64)

# Sets BZR_REVNO
execute_process(
    COMMAND bzr revno ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE BZR_REVNO
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(DATA_DIR ${CMAKE_INSTALL_FULL_DATADIR}/${APP_NAME})
set(ICON ${DATA_DIR}/${ICON_FILE})
set(QT_IMPORTS_DIR ${DATA_DIR}/private/qml/)
set(DESKTOP_DIR ${CMAKE_INSTALL_DATADIR}/applications)
install(FILES ${CONTENT_HUB_JSON} DESTINATION ${CMAKE_INSTALL_FULL_DATADIR}/content-hub/peers/ RENAME ubuntu-filemanager-app)

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

configure_file("${CMAKE_CURRENT_SOURCE_DIR}/config.h.in"
    "${CMAKE_CURRENT_BINARY_DIR}/config.h"
    IMMEDIATE @ONLY)

# Tests
enable_testing()

add_subdirectory(po)
add_subdirectory(tests)
add_subdirectory(src)
