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
import Ubuntu.Components 1.3
import QtGraphicalEffects 1.0

MouseArea {
    id: delegate

    property bool isSelected

    property string iconName
    property string title
    property string path

    // Ensure that file name can overlap the items below.
    z: delegate.containsMouse ? 10 : 0

    Rectangle {
        anchors { fill: parent; margins: units.dp(1) }
        radius: units.dp(8)
        opacity: delegate.isSelected ? 0.5 : 0
        color: UbuntuColors.green

        Behavior on opacity {
            UbuntuNumberAnimation {}
        }
    }

    Item {
        id: imgContainer
        anchors { top: parent.top; topMargin: units.gu(1) }
        anchors.horizontalCenter: parent.horizontalCenter
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

        /*BrightnessContrast {
            anchors.fill: imgContainer
            brightness: 0.3
            source: imgContainer
            opacity: visible ? 1.0 : 0.0
            visible: delegate.containsMouse

            Behavior on opacity {
                UbuntuNumberAnimation {}
            }
        }*/
    }

    Label {
        anchors {
            left: parent.left; leftMargin: units.gu(0.5)
            right: parent.right; rightMargin: units.gu(0.5)
            top: imgContainer.bottom; topMargin: units.gu(1)
        }

        horizontalAlignment: Text.AlignHCenter
        color: theme.palette.normal.backgroundSecondaryText

        text: delegate.title

        // TODO: This is probably too small, but this way we are sure that text has always
        // a good contrast with bg, since it doesn't overlay other icons or strings.
        textSize: Label.Small

        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        elide: Text.ElideMiddle
        maximumLineCount: delegate.containsMouse ? Number.MAX_VALUE : 3

        style: delegate.containsMouse && !delegate.isSelected ? Text.Outline : Text.Normal
        styleColor: theme.palette.normal.background
    }

    acceptedButtons: (Qt.LeftButton | Qt.RightButton)
    hoverEnabled: true

    GridView.onRemove: SequentialAnimation {
        PropertyAction { target: delegate; property: "GridView.delayRemove"; value: true }
        NumberAnimation { target: delegate; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
        PropertyAction { target: delegate; property: "GridView.delayRemove"; value: false }
    }
}
