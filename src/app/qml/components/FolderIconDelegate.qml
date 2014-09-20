/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Michael Spencer <sonrisesoftware@gmail.com>
 */
import QtQuick 2.3
import QtGraphicalEffects 1.0
import Ubuntu.Components 1.1

Item {
    id: delegate
    height: units.gu(11)
    width: units.gu(11)

    property bool selected
    property alias mouseOver: mouseArea.containsMouse

    Rectangle {
        anchors.centerIn: parent
       // anchors.verticalCenterOffset: units.gu(0.5)
        height: parent.height// + units.gu(1)
        width: height

        radius: units.gu(2)
        smooth: true
        antialiasing: true
        opacity: model.isSelected ? 0.5 : 0
        color: UbuntuColors.orange

        Behavior on opacity {
            UbuntuNumberAnimation {}
        }
    }

    objectName: "folder" + index

    property string fileName: model.fileName
    property string filePath: fileView.folder + '/' + fileName

    property string text: fileName
    property string subText: Qt.formatDateTime(model.modifiedDate, Qt.DefaultLocaleShortDate) + (!model.isDir ? ", " + fileSize : "")

    property var icon: fileIcon(filePath, model.isDir)

    Item {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
            bottom: label.top
            topMargin: units.gu(0.5)
            bottomMargin: units.gu(1)
            leftMargin: units.gu(1)
            rightMargin: units.gu(1)
        }

        Image {
            id: image
            anchors.centerIn: parent
            width: units.gu(6)
            height: width

            source: delegate.icon
        }

        BrightnessContrast {
            anchors.fill: image
            brightness: 0.3
            source: image
            opacity: mouseOver ? 1 : 0

            Behavior on opacity {
                UbuntuNumberAnimation {}
            }
        }
    }

    property bool expand: mouseOver && label.implicitWidth >= label.width
    z: expand ? 1 : 0

    Rectangle {
        anchors {
            fill: label
            margins: units.gu(-0.5)
            leftMargin: units.gu(-1)
            rightMargin: units.gu(-1)
        }
        color: "white"
        radius: height/2
        border.color: UbuntuColors.warmGrey
        antialiasing: true
        opacity: expand ? 1 : 0

        Behavior on opacity {
            UbuntuNumberAnimation {}
        }
    }

    Label {
        id: label
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            bottomMargin: units.gu(0.75)
        }

        width: expand ? implicitWidth : (parent.width - units.gu(0.5))

        Behavior on width {
            UbuntuNumberAnimation {}
        }

        horizontalAlignment: Text.AlignHCenter
        elide: Text.ElideMiddle

        text: delegate.text
        color: expand ? UbuntuColors.coolGrey : Theme.palette.selected.backgroundText
        Behavior on color {
            ColorAnimation { duration: 200 }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: (Qt.LeftButton | Qt.RightButton)

        onClicked: {
            if (mouse.button === Qt.RightButton)
                pressAndHold(mouse)
            else
                itemClicked(model)
        }

        propagateComposedEvents: true

        onPressAndHold: itemLongPress(delegate, model)
    }
}
