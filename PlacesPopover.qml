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
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1

Popover {
    id: root
    objectName: "placesPopover"

    ListModel {
        id: places

        ListElement {
            path: "~"
            icon: "/usr/share/icons/ubuntu-mono-dark/places/48/folder-home.svg"
        }

        ListElement {
            path: "~/Documents"
            icon: "/usr/share/icons/Humanity/places/48/folder-documents.svg"
        }

        ListElement {
            path: "~/Downloads"
            icon: "/usr/share/icons/Humanity/places/48/folder-downloads.svg"
        }

        ListElement {
            path: "~/Music"
            icon: "/usr/share/icons/Humanity/places/48/folder-music.svg"
        }

        ListElement {
            path: "~/Pictures"
            icon: "/usr/share/icons/Humanity/places/48/folder-pictures.svg"
        }

        ListElement {
            path: "~/Videos"
            icon: "/usr/share/icons/Humanity/places/48/folder-videos.svg"
        }

        ListElement {
            path: "/"
            icon: "/usr/share/icons/Humanity/devices/48/drive-harddisk.svg"
        }
    }

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        Repeater {
            id: placesList

            model: places

            delegate: Standard {
                text: folderName(path)
                icon: model.icon || "/usr/share/icons/Humanity/places/48/folder.svg"

                onClicked: {
                    PopupUtils.close(root)
                    goTo(model.path)
                }
            }
        }
    }
}
