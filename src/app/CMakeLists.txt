file(GLOB_RECURSE QML_SRCS *.qml *.js)


include_directories(${CMAKE_BINARY_DIR})

set(filemanager_SRCS
    main.cpp
    ${QML_SRCS}
)

add_executable(ubuntu-filemanager-app ${filemanager_SRCS})

qt5_use_modules(ubuntu-filemanager-app Gui Qml Quick)

if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
add_custom_target(filemanager-qmlfiles ALL
    COMMAND cp -r ${CMAKE_CURRENT_SOURCE_DIR}/qml ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${QMLFILES}
)
endif(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")

install(DIRECTORY qml DESTINATION ${DATA_DIR})
install(TARGETS ubuntu-filemanager-app RUNTIME DESTINATION bin)
