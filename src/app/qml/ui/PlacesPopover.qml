/*
 * Copyright (C) 2013, 2014 Canonical Ltd
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
import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0

Popover {
    id: root
    objectName: "placesPopover"

    Column {
        // Places must not be visible when virtual keyboard is open, because then
        // the virtual keyboard can push the text input out of visible area and
        // you don't see what you type. So when virtual keyboard is open we show
        // only the text input and a "Places" icon that you can click to get the
        // full list again.
        Connections {
            id: placesVisibleController
            target: Qt.inputMethod
            onVisibleChanged: {
                showPlaces.visible = Qt.inputMethod.visible
                placesList.visible = !Qt.inputMethod.visible
            }
        }

        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        Empty {

            TextField {
                id: locationField
                objectName: "inputField"
                anchors {
                    verticalCenter: parent.verticalCenter
                    left: parent.left
                    right: goButton.left
                    margins: units.gu(1)
                }

                inputMethodHints: Qt.ImhNoAutoUppercase

                property bool valid: pathExists(text)

                text: fileView.folder

                placeholderText: i18n.tr("Location...")

                onAccepted: goButton.clicked()
            }

            Button {
                id: goButton
                objectName: "okButton"
                anchors {
                    top: locationField.top
                    bottom: locationField.bottom
                    right: parent.right
                    rightMargin: units.gu(1)
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

        Empty {
            id: showPlaces
            visible: false
            Standard {
                objectName: "showPlaces"

                Label {
                    anchors.left: parent.left
                    anchors.leftMargin: units.gu(8)
                    anchors.verticalCenter: parent.verticalCenter
                    text: i18n.tr("Places")
                    color: Theme.palette.normal.overlayText
                }

                onClicked: {
                    locationField.activeFocus = false
                }

                iconSource: getIcon("location")

                iconFrame: false
            }
        }

        Repeater {
            id: placesList
            objectName: "placesList"
            visible: true
            model: userplaces

            delegate: Standard {
                visible: placesList.visible
                objectName: "place" + folderDisplayName(path).replace(/ /g,'')
                property string name: folderDisplayName(path)

                Label {
                    anchors.left: parent.left
                    anchors.leftMargin: units.gu(8)
                    anchors.verticalCenter: parent.verticalCenter
                    text: folderDisplayName(path)
                    color: selected ? UbuntuColors.orange : Theme.palette.normal.overlayText
                }

                iconSource: model.icon || fileIcon(model.path, true)

                onClicked: {
                    PopupUtils.close(root)
                    goTo(model.path)
                }

                selected: folder === path
                iconFrame: false
                showDivider: index < (placesList.count - 1)
            }
        }
    }
}
