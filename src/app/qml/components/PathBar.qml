import QtQuick 2.0
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import com.ubuntu.PlacesModel 0.1

Rectangle {
    id: root
    objectName: "pathbar"
    radius: units.gu(0.5)
    border.color: UbuntuColors.warmGrey

    antialiasing: true
    anchors.verticalCenter: parent.verticalCenter

    implicitWidth: goToButton.width + row.width

    function rootDisplayName(path) {
        // Returns the human-readable name for the root of a filesystem location
        // for display purposes
        // Supported roots are /home/$USER and /

        var pathDisplayName

        if ((path.substring(0, userplaces.locationHome.length) === userplaces.locationHome) &&
            ((path.substring(userplaces.locationHome.length, userplaces.locationHome.length + 1) === '/') ||
             (path.length === userplaces.locationHome.length))) {
            // Replace /home/$USER by its human readable name (generally "Home")
            pathDisplayName = path.replace(userplaces.locationHome, folderDisplayName(userplaces.locationHome))
        } else {
            // Replace any other path than /home/$USER with the human readable of the root
            // file system (generally "Device")
            pathDisplayName = folderDisplayName("/").concat(path.replace(/^\/$/, ""))
        }

        return pathDisplayName
    }

    function rootAbsName(path) {
        // Returns the absolute path name given a human-readable path
        // Does the opposite of the rootDisplayName function

        var pathAbsName
        var rootAbsName
        var pathComponents = path.split("/")

        // Get the absolute path of the root
        if (pathComponents[0] === folderDisplayName(userplaces.locationHome)) {
            rootAbsName = userplaces.locationHome
        } else {
            rootAbsName = "/"
        }

        // Drop the human-readable part of the path components
        pathComponents.shift()
        // Join the root absolute path name with the rest of the path components,
        // avoiding "/" duplicates
        pathAbsName = rootAbsName.concat("/", pathComponents.join("/")).replace(/^\/\//, "/")

        return pathAbsName
    }

    Behavior on width {
        UbuntuNumberAnimation {}
    }

    Rectangle {
        id: goToButton
        objectName: "goToButton"
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }

        MouseArea {
            id: mouseArea2
            anchors.fill: parent
            hoverEnabled: true

            onClicked: {
                PopupUtils.open(Qt.resolvedUrl("../ui/GoToDialog.qml"), goToButton)
            }
        }

        color: mouseArea2.containsMouse ? Qt.rgba(0.5,0.5,0.5,0.2) : Qt.rgba(0.5,0.5,0.5,0)

        Behavior on color {
            ColorAnimation { duration: 200 }
        }

        width: parent.height

        Image {
            id: editImage
            anchors.centerIn: parent

            source: getIcon("edit")
            height: units.gu(2)
            width: height
        }

        Rectangle {
            anchors {
                left: parent.left
                top: parent.top
                bottom: parent.bottom
                rightMargin: units.gu(1)
            }

            width: 1
            color: UbuntuColors.warmGrey
        }
    }

    Flickable {
        anchors {
            left: parent.left
            right: goToButton.left
            top: parent.top
            bottom: parent.bottom
        }

        clip: true

        flickableDirection: Flickable.HorizontalFlick
        contentWidth: row.width
        contentHeight: parent.height

        Row {
            id: row
            height: root.height
            width: implicitWidth - 1

            Repeater {
                id: repeater
                // This refers to a parent FolderListPage.folder
                model: rootDisplayName(folder).split("/")
                delegate: Rectangle {
                    MouseArea {
                        id: mouseArea
                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            var upCount = index
                            var path = modelData

                            while (upCount > 0) {
                                path = repeater.model[--upCount] + "/" + path
                            }

                            goTo(rootAbsName(path))
                        }
                    }

                    color: mouseArea.containsMouse ? Qt.rgba(0.5,0.5,0.5,0.2) : Qt.rgba(0.5,0.5,0.5,0)

                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }

                    Label {
                        id: label
                        anchors.centerIn: parent
                        text: modelData === "" ? "/" : modelData
                        color: UbuntuColors.coolGrey
                    }

                    height: row.height
                    width: label.width + units.gu(4)

                    Rectangle {
                        anchors {
                            right: parent.right
                            top: parent.top
                            bottom: parent.bottom
                        }
                        width: 1
                        color: UbuntuColors.warmGrey
                    }
                }
            }
        }
    }
}
