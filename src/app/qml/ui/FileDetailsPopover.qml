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
        return Qt.formatDateTime(dateTime, Qt.DefaultLocaleShortDate) || "Unknown"
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
        }

        ListItemLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            title.text: folderModel.pathTitle(model.filePath)
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

    RowLayout {
        anchors { left: parent.left; right: parent.right }
        anchors.leftMargin: units.gu(-2)
        anchors.rightMargin: units.gu(-2)

        ListItemLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            subtitle.text: i18n.tr("Accessed:")
            summary.text: dateTimeFormat(model.accessedDate)
        }

        ListItemLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            subtitle.text: i18n.tr("Modified:")
            summary.text: dateTimeFormat(model.modifiedDate)
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
}
