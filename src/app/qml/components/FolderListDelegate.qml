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
 * Authored by: Arto Jalkanen <ajalkane@gmail.com>
 */
import QtQuick 2.4
import Ubuntu.Components 1.3
import org.nemomobile.folderlistmodel 1.0

ListItem {
    id: del

    property string title
    property string subtitle
    property string iconName
    property string path
    property bool showProgressionSlot
    property bool isSelected

    // Because Flickable is used over ListView??
    // we cannot set the highlight component so
    // instead fudge it here with a rectangle.
    Rectangle {
        anchors.fill: parent
        color: UbuntuColors.silk
        visible: del.isSelected
    }

    ListItemLayout {
        id: layout
        title.text: del.title
        subtitle.text: del.subtitle

        Item {
            SlotsLayout.position: SlotsLayout.Leading
            height: units.gu(5); width: height

            Icon {
                anchors.fill: parent
                visible: !image.visible
                name: del.iconName
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

        ProgressionSlot{
            visible: del.showProgressionSlot
        }
    }

    ListView.onRemove: SequentialAnimation {
        PropertyAction { target: del; property: "ListView.delayRemove"; value: true }
        NumberAnimation { target: del; property: "scale"; to: 0; duration: 250; easing.type: Easing.InOutQuad }
        PropertyAction { target: del; property: "ListView.delayRemove"; value: false }
    }
}
