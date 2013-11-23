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
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem

Item {
    id: root

    property var folderListModel
    property string folderPath: folderListModel.path
    property int count: repeater.count
    property Flickable flickable: flickable
    property bool smallMode: false

    ListItem.Header {
        id: header
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        objectName: "iconViewHeader"
        text: (root.count == 1
               ? i18n.tr("%1 (1 file)").arg(root.folderPath)
               : i18n.tr("%1 (%2 files)").arg(root.folderPath).arg(root.count))
        visible: !smallMode
        height: visible ? units.gu(4) : 0
    }

    Flickable {
        id: flickable
        clip: true
        anchors {
            top: header.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }

        contentWidth: width
        contentHeight: grid.height

        Column {
            width: flickable.contentWidth
            ListItem.Header {
                anchors {
                    left: parent.left
                    right: parent.right
                }

                objectName: "iconViewSmallHeader"
                text: (root.count == 1
                       ? i18n.tr("%1 (1 file)").arg(root.folderPath)
                       : i18n.tr("%1 (%2 files)").arg(root.folderPath).arg(root.count))
                visible: smallMode
                height: visible ? units.gu(4) : 0
            }

            AutoSpacedGrid {
                id: grid
                width: root.width

                cellCount: repeater.count
                cellWidth: units.gu(8)
                cellHeight: units.gu(10)
                minSpacing: units.gu(5)
                ySpacing: 1/2 * spacing

                Repeater {
                    id: repeater
                    model: folderListModel
                    delegate: FolderIconDelegate {

                    }
                }
            }
        }
    }

    Scrollbar {
        flickableItem: flickable
    }
}
