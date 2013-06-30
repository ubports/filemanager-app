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
        return Qt.formatDateTime(dateTime, Qt.DefaultLocaleShortDate) || "Uknown"
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
                font.bold: true
            }
        }

        Grid {
            columns: 2
            spacing: units.gu(1)

            Label {
                text: i18n.tr("Path:")
            }

            Label {
                text: root.path
            }

            Label {
                text: model.isDir ? i18n.tr("Contents:")
                                  : i18n.tr("Size:")
            }
            Label {
                text: model.fileSize
            }

            Label {
                text: i18n.tr("Created:")
            }
            Label {
                text: dateTimeFormat(model.creationDate)
            }

            Label {
                text: i18n.tr("Modified:")
            }
            Label {
                text: dateTimeFormat(model.modifiedDate)
            }

            Label {
                text: i18n.tr("Permissions:")
            }
            Label {
                text: permissionsToString(model)
            }

        }
    }
}
