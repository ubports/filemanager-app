import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

Rectangle {
    id: root
    radius: units.gu(0.5)
    border.color: UbuntuColors.warmGrey

    antialiasing: true
    anchors.verticalCenter: parent.verticalCenter

    Rectangle {
        id: goToButton
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
                PopupUtils.open(Qt.resolvedUrl("GoToDialog.qml"), goToButton)
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
                model: folder === "/" ? [""] : folder.split("/")
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
                                //print(path)
                            }

                            if (path === "") path = "/"

                            goTo(path)
                        }
                    }

                    color: mouseArea.containsMouse ? Qt.rgba(0.5,0.5,0.5,0.2) : Qt.rgba(0.5,0.5,0.5,0)

                    Behavior on color {
                        ColorAnimation { duration: 200 }
                    }

                    Label {
                        id: label
                        anchors.centerIn: parent
                        text: modelData === "" ? "/" : modelData === "~" ? i18n.tr("Home") : modelData
                        color: UbuntuColors.coolGrey
                    }

                    height: parent.height
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
