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
import QtQuick 2.4
import QtGraphicalEffects 1.0
import Ubuntu.Components 1.3

// TODO: Check color usage
// TODO: Check design

Item {
    id: delegate

    property bool isSelected
    property alias mouseOver: mouseArea.containsMouse

    property string iconName
    property string title
    property string path

    signal clicked(var mouse)
    signal pressAndHold(var mouse)

    Rectangle {
        anchors.centerIn: parent
        height: parent.height; width: height
        radius: units.dp(8)
        opacity: delegate.isSelected ? 0.5 : 0
        color: UbuntuColors.orange

        Behavior on opacity {
            UbuntuNumberAnimation {}
        }
    }

    Item {
        anchors {
            left: parent.left; leftMargin: units.gu(1)
            right: parent.right; rightMargin: units.gu(1)
            top: parent.top; topMargin: units.gu(0.5)
            bottom: label.top; bottomMargin: units.gu(1)
        }

        Item {
            id: imgContainer
            anchors.centerIn: parent
            height: units.gu(6); width: height

            Icon {
                anchors.fill: parent
                visible: !image.visible
                name: delegate.iconName
            }

            Image {
                id: image
                anchors.fill: parent
                sourceSize: Qt.size(image.width, image.height)
                visible: status == Image.Ready

                source: model.mimeType.indexOf("image/") > -1 ? "image://thumbnailer/file://" + delegate.path : ""
                fillMode: Image.PreserveAspectFit
                asynchronous: true
            }
        }

        // TODO: Check performance, QtGraphicalEffects in a delegate might become a problem
        BrightnessContrast {
            anchors.fill: imgContainer
            brightness: 0.3
            source: imgContainer
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
        radius: units.dp(4)
        border.color: UbuntuColors.slate
        border.width: units.dp(1)

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

        text: delegate.title
        color: UbuntuColors.graphite
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        hoverEnabled: true
        acceptedButtons: (Qt.LeftButton | Qt.RightButton)
        propagateComposedEvents: true

        onClicked: delegate.clicked(mouse)
        onPressAndHold: delegate.pressAndHold(mouse)
    }

    GridView.onRemove: SequentialAnimation {
        PropertyAction { target: delegate; property: "GridView.delayRemove"; value: true }
        NumberAnimation { target: delegate; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
        PropertyAction { target: delegate; property: "GridView.delayRemove"; value: false }
    }
}
