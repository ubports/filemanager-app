# Disable TagLib in folderlistmodel plugin
ADD_DEFINITIONS(-DDO_NOT_USE_TAG_LIB)

include(FindPkgConfig)
find_package(Qt5Core)
find_package(Qt5Qml)
find_package(Qt5Quick)
find_package(Qt5Widgets)

add_subdirectory(folderlistmodel)
add_subdirectory(placesmodel)
add_subdirectory(pamauthentication)
add_subdirectory(archives)
