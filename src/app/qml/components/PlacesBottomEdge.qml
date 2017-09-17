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
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import org.nemomobile.folderlistmodel 1.0

BottomEdge {
    id: bottomEdge

    hint {
        iconName: "location"
        text: i18n.tr("Places")
        enabled: visible
        visible: bottomEdge.enabled
    }

    contentComponent: Page {
        id: root
        width: bottomEdge.width
        height: bottomEdge.height

        // ADDED THIS PROPERTY FOR BUG #1341671  WORKAROUND
        property string testProperty: "justForAutopilotTests"
        objectName: 'PlacesPage'

        header: PageHeader {
            title: i18n.tr("Places")
        }

        ScrollView {
            id: scrollView
            objectName: "placesFlickable"
            anchors.fill: parent
            anchors.topMargin: root.header.height

            Column {
                id: content
                width: scrollView.width

                ListItem {
                    TextField {
                        id: locationField
                        objectName: "placePath"
                        anchors {
                            verticalCenter: parent.verticalCenter
                            left: parent.left
                            right: goButton.left
                            margins: units.gu(1)
                        }

                        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText

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
                        color: UbuntuColors.green
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

                    ListItem {
                        objectName: "place" + folderDisplayName(model.path).replace(/ /g,'')
                        property string name: folderDisplayName(model.path)

                        Rectangle {
                            anchors.fill: parent
                            color: Qt.rgba(0, 0, 0, 0.2)
                            visible: model.path == folderListPage.folder
                        }

                        ListItemLayout {
                            anchors.fill: parent
                            title.text: folderDisplayName(model.path)

                            Icon {
                                SlotsLayout.position: SlotsLayout.Leading
                                width: units.gu(4); height: width
                                source: model.icon || fileIcon(model.path, true)
                            }
                        }

                        onClicked: {
                            goTo(model.path)
                            bottomEdge.collapse()
                        }
                    }
                }
            }
        }
    }
}
