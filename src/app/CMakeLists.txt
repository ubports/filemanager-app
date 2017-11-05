file(GLOB_RECURSE QML_SRCS *.qml *.js *.png *.svg)

set(filemanager_SRCS
    main.cpp
    patharrowbackground.cpp
    patharrowbackground.h
    fmutils.cpp
    fmutils.h
    ${QML_SRCS}
)

add_executable(filemanager ${filemanager_SRCS})

qt5_use_modules(filemanager Gui Qml Quick)
target_link_libraries(filemanager stdc++)

if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
add_custom_target(filemanager-qmlfiles ALL
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/qml ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${QMLFILES}
)
endif(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")

install(DIRECTORY qml DESTINATION ${DATA_DIR})
if(CLICK_MODE)
  install(TARGETS filemanager DESTINATION ${BIN_DIR})
else()
  install(TARGETS filemanager RUNTIME DESTINATION bin)
endif()
