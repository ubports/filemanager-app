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
 * Authored by: Michael Spencer <spencers1993@gmail.com>
 */
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1
import Ubuntu.Components.Popups 0.1

Sidebar {
    id: root

    width: units.gu(25)

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
            text: i18n.tr("Places")
        }

        Repeater {
            id: placesList
            objectName: "placesList"

            model: places

            delegate: Standard {
                text: folderName(path)
                icon: model.icon || fileIcon(model.path, true)

                onClicked: {
                    goTo(model.path)
                }

                height: units.gu(5)

                selected: folder === path
            }
        }
    }
}
