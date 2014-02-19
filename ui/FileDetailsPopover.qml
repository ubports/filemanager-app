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
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

Popover {
    id: root
    property var model

    property string path: model.path || (fileView.path + '/' + model.fileName)

    contentHeight: contents.height + 2 * contents.anchors.margins

    function dateTimeFormat(dateTime) {
        return Qt.formatDateTime(dateTime, Qt.DefaultLocaleShortDate) || "Unknown"
    }

    function permissionsToString(model) {
        var permissions = []
        if (model.isReadable) {
            permissions.push(i18n.tr("Readable"))
        }
        if (pathIsWritable()) {
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

    Column {
        id: contents
        spacing: units.gu(1)
        anchors {
            margins: units.gu(2)
            left: parent.left
            right: parent.right
            top: parent.top
        }

        Row {
            spacing: units.gu(1)
            Image {
                anchors.verticalCenter: parent.verticalCenter

                // TODO: how to get proper icon?
                source: fileIcon(root.path, model.isDir)
            }

            Label {
                anchors.verticalCenter: parent.verticalCenter

                text: folderName(root.path)
                color: Theme.palette.normal.overlayText
                font.bold: true
            }
        }

        Grid {
            columns: 2
            spacing: units.gu(1)

            Label {
                text: i18n.tr("Path:")
                color: Theme.palette.normal.overlayText
            }

            Label {
                objectName: "pathLabel"
                text: root.path
                color: Theme.palette.normal.overlayText
            }

            Label {
                text: model.isDir ? i18n.tr("Contents:")
                                  : i18n.tr("Size:")
                color: Theme.palette.normal.overlayText
            }
            Label {
                text: model.fileSize
                color: Theme.palette.normal.overlayText
            }

            Label {
                text: i18n.tr("Accessed:")
                color: Theme.palette.normal.overlayText
            }
            Label {
                text: dateTimeFormat(pathAccessedDate())
                color: Theme.palette.normal.overlayText
            }

            Label {
                text: i18n.tr("Modified:")
                color: Theme.palette.normal.overlayText
            }
            Label {
                text: dateTimeFormat(pathModifiedDate())
                color: Theme.palette.normal.overlayText
            }

            Label {
                text: i18n.tr("Permissions:")
                color: Theme.palette.normal.overlayText
            }
            Label {
                text: permissionsToString(model)
                color: Theme.palette.normal.overlayText
            }

        }
    }
}
