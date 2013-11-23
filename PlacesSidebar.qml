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
import Ubuntu.Components.ListItems 0.1
import Ubuntu.Components.Popups 0.1

Sidebar {
    id: root

    //color: Qt.rgba(0.5,0.5,0.5,0.3)
    width: units.gu(30)

    ListModel {
        id: places

        ListElement {
            path: "~"
        }

        ListElement {
            path: "~/Documents"
        }

        ListElement {
            path: "~/Downloads"
        }

        ListElement {
            path: "~/Music"
        }

        ListElement {
            path: "~/Pictures"
        }

        ListElement {
            path: "~/Videos"
        }

        ListElement {
            path: "/"
        }
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        Header {
//            Label {
//                anchors.left: parent.left
//                anchors.leftMargin: units.gu(1)
//                anchors.verticalCenter: parent.verticalCenter
                text: i18n.tr("Places")
//                color: Theme.palette.normal.overlayText
//            }
        }

        Repeater {
            id: placesList
            objectName: "placesList"

            model: places

            delegate: Standard {
//                Label {
//                    anchors.left: parent.left
//                    anchors.leftMargin: units.gu(8)
//                    anchors.verticalCenter: parent.verticalCenter
                    text: folderName(path)
//                    color: Theme.palette.normal.overlayText
//                }
                icon: model.icon || fileIcon(model.path, true)

                onClicked: {
                    goTo(model.path)
                }

                height: units.gu(5)

                selected: folder === path
                iconFrame: false
            }
        }
    }
}
