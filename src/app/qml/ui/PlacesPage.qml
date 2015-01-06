/*
 * Copyright (C) 2014 Canonical Ltd
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
import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0
import org.nemomobile.folderlistmodel 1.0

Page {
    id: root

    title: i18n.tr("Places")
    objectName: 'PlacesPage'
    // ADDED THIS PROPERTY FOR BUG #1341671  WORKAROUND
    property string testProperty: "justForAutopilotTests"

    Flickable {
        objectName: "placesFlickable"
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            Empty {

                TextField {
                    id: locationField
                    objectName: "placePath"
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
                        pageStack.pop()
                    }
                }
            }

            Repeater {
                id: placesList
                objectName: "placesList"
                model: userplaces

                delegate: Standard {
                    objectName: "place" + folderDisplayName(path).replace(/ /g,'')
                    property string name: folderDisplayName(path)

                    Label {
                        anchors.left: parent.left
                        anchors.leftMargin: units.gu(8)
                        anchors.verticalCenter: parent.verticalCenter
                        text: folderDisplayName(path)
                    }

                    iconSource: model.icon || fileIcon(model.path, true)

                    onClicked: {
                        goTo(model.path)
                        pageStack.pop()
                    }

                    selected: folder === path
                    iconFrame: false
                    showDivider: index < (placesList.count - 1)
                }
            }
        }
    }

}
