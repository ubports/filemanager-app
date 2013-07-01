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

        Repeater {
            id: placesList
            objectName: "placesList"

            model: places

            delegate: Standard {
                text: folderName(path)
                icon: model.icon || fileIcon(model.path, true)

                onClicked: {
                    PopupUtils.close(root)
                    goTo(model.path)
                }
            }
        }

        Empty {

            TextField {
                id: locationField
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: goButton.left
                    margins: units.gu(1)
                }

                inputMethodHints: Qt.ImhNoAutoUppercase

                property bool valid: pathExists(text)

                text: fileView.path

                placeholderText: i18n.tr("Location...")

                onAccepted: goButton.clicked()
            }

            Button {
                id: goButton
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: units.gu(1)
                }

                text: i18n.tr("Go")
                enabled: locationField.acceptableInput && locationField.valid

                onClicked: {
                    print("User switched to:", locationField.text)
                    goTo(locationField.text)
                    PopupUtils.close(root)
                }
            }
        }
    }
}
