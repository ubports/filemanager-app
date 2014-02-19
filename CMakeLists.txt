project(com.ubuntu.filemanager)
cmake_minimum_required(VERSION 2.8.9)

# Standard install paths
include(GNUInstallDirs)

option(INSTALL_TESTS "Install the tests on make install" on)
option(CLICK_MODE "Installs to a contained location" off)

set(APP_NAME filemanager)
set(DESKTOP_FILE "${PROJECT_NAME}_${APP_NAME}.desktop")
set(URLS_FILE "${PROJECT_NAME}_${APP_NAME}.url-dispatcher")
set(APP_HARDCODE ubuntu-filemanager-app)
set(MAIN_QML ${APP_HARDCODE}.qml)
set(ICON filemanager64.png)
set(AUTOPILOT_DIR ubuntu_filemanager_app)

if(CLICK_MODE)
    if(NOT DEFINED BZR_SOURCE)
        set(BZR_SOURCE "lp:${APP_HARDCODE}")
        message("-- Setting BZR_SOURCE to ${BZR_SOURCE}")
    endif(NOT DEFINED BZR_SOURCE)
    set(CMAKE_INSTALL_PREFIX /)
    set(CMAKE_INSTALL_BINDIR /)
    set(DATA_DIR /)
    set(EXEC "qmlscene ${MAIN_QML}")
    set(DESKTOP_DIR ${DATA_DIR})
    set(URLS_DIR ${DATA_DIR})
else(CLICK_MODE)
    set(DATA_DIR ${CMAKE_INSTALL_DATADIR}/${APP_HARDCODE})
    set(EXEC ${APP_HARDCODE})
    set(ICON ${CMAKE_INSTALL_PREFIX}/${DATA_DIR}/${ICON})
    configure_file(${APP_HARDCODE}.in
        ${CMAKE_CURRENT_BINARY_DIR}/${APP_HARDCODE})
    install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/${APP_HARDCODE}
        DESTINATION ${CMAKE_INSTALL_BINDIR})
    set(DESKTOP_DIR ${CMAKE_INSTALL_DATADIR}/applications)
    set(URLS_DIR ${CMAKE_INSTALL_DATADIR}/url-dispatcher/urls)
endif(CLICK_MODE)

file(GLOB_RECURSE I18N_SRC_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     rc/**.qml desktop/**.desktop.in)
list(SORT I18N_SRC_FILES)


file(GLOB SRC_FILES
     RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
     *.qml *.js *.png *.js *.json)
install(DIRECTORY ui components icons
    DESTINATION ${DATA_DIR})
install(FILES ${MAIN_QML} DESTINATION ${DATA_DIR})

configure_file(${DESKTOP_FILE}.in ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${DESKTOP_FILE}
    DESTINATION ${DESKTOP_DIR})

install(FILES ${URLS_FILE} DESTINATION ${URLS_DIR})

# Tests
enable_testing()

add_subdirectory(click)
add_subdirectory(po)
add_subdirectory(tests)
