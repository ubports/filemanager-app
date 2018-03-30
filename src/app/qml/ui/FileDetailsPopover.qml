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
import Ubuntu.Components.Popups 1.3
import QtQuick.Layouts 1.1

Dialog {
    id: root
    property var model

    function dateTimeFormat(dateTime) {
        return Qt.formatDateTime(dateTime, Qt.TextDate) || "Unknown"
    }

    function permissionsToString(model) {
        var permissions = []
        if (model.isReadable) {
            permissions.push(i18n.tr("Readable"))
        }
        if (model.isWritable) {
            permissions.push(i18n.tr("Writable"))
        }
        if (model.isExecutable) {
            permissions.push(i18n.tr("Executable"))
        }

        // Now why does this not work?
        // return permissions.join(", ")
        var permStr = ""
        for (var i = 0; i < permissions.length; ++i) {
            if (permStr.length > 0) {
                permStr += ", "
            }
            permStr += permissions[i]
        }
        return permStr
    }

    __closeOnDismissAreaPress: true

    Component.onCompleted: {
        __foreground.itemSpacing = units.gu(0)
    }

    RowLayout {
        Icon {
            Layout.preferredWidth: units.gu(6)
            Layout.preferredHeight: width
            name: model.iconName
            visible: !image.visible
        }

        Image {
            id: image
            Layout.preferredWidth: units.gu(6)
            Layout.preferredHeight: width
            sourceSize: Qt.size(units.gu(6), width)
            visible: status == Image.Ready

            source: model.mimeType.indexOf("image/") > -1 ? model.filePath : ""
            fillMode: Image.PreserveAspectFit
            asynchronous: true
        }

        ListItemLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title.text: FmUtils.pathTitle(model.filePath)
            subtitle.text: model.mimeTypeDescription
            summary.text: model.fileSize
        }
    }

    Rectangle {
        anchors { left: parent.left; right: parent.right }
        height: units.dp(1)
        color: theme.palette.normal.base
    }

    ListItemLayout {
        anchors { left: parent.left; right: parent.right }
        anchors.leftMargin: units.gu(-2)
        anchors.rightMargin: units.gu(-2)
        subtitle.text: i18n.tr("Where:")
        summary.maximumLineCount: Number.MAX_VALUE
        summary.wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        summary.text: model.filePath.toString().replace(/^\//, i18n.tr("My Device") + " > ").replace(/\//g, " > ")
    }

    Rectangle {
        anchors { left: parent.left; right: parent.right }
        height: units.dp(1)
        color: theme.palette.normal.base
    }

    ListItem {
        divider.visible: false
        height: dateGrid.height + units.gu(4)

        GridLayout {
            id: dateGrid
            anchors.verticalCenter: parent.verticalCenter
            columns: 2
            rowSpacing: units.dp(2)

            // Row #1
            Label {
                Layout.fillWidth: true
                textSize: Label.Small
                color: theme.palette.normal.backgroundSecondaryText
                text: i18n.tr("Created:")
            }

            Label {
                textSize: Label.Small
                color: theme.palette.normal.backgroundTertiaryText
                text: dateTimeFormat(model.creationDate)
            }

            // Row #2
            Label {
                Layout.fillWidth: true
                textSize: Label.Small
                color: theme.palette.normal.backgroundSecondaryText
                text: i18n.tr("Modified:")
            }

            Label {
                textSize: Label.Small
                color: theme.palette.normal.backgroundTertiaryText
                text: dateTimeFormat(model.modifiedDate)
            }

            // Row #3
            Label {
                Layout.fillWidth: true
                textSize: Label.Small
                color: theme.palette.normal.backgroundSecondaryText
                text: i18n.tr("Accessed:")
            }

            Label {
                textSize: Label.Small
                color: theme.palette.normal.backgroundTertiaryText
                text: dateTimeFormat(model.accessedDate)
            }
        }
    }

    Rectangle {
        anchors { left: parent.left; right: parent.right }
        height: units.dp(1)
        color: theme.palette.normal.base
    }

    ListItemLayout {
        anchors { left: parent.left; right: parent.right }
        anchors.leftMargin: units.gu(-2)
        anchors.rightMargin: units.gu(-2)
        subtitle.text: i18n.tr("Permissions:")
        summary.maximumLineCount: Number.MAX_VALUE
        summary.wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        summary.text: permissionsToString(model)
    }

    Button {
        id: cancelButton
        text: i18n.tr("Cancel")
        color: UbuntuColors.graphite
        onClicked: {
            PopupUtils.close(root)
        }
    }
}
