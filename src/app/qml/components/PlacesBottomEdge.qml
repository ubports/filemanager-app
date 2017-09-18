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

    property var folderListModel

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

            ListView {
                anchors.fill: parent
                model: userplaces

                delegate: ListItem {
                    objectName: "place" + folderDisplayName(model.path).replace(/ /g,'')
                    property string name: folderDisplayName(model.path)

                    Rectangle {
                        anchors.fill: parent
                        color: Qt.rgba(0, 0, 0, 0.2)
                        visible: model.path == folderListModel.path
                    }

                    ListItemLayout {
                        anchors.fill: parent
                        title.text: folderDisplayName(model.path)

                        Icon {
                            SlotsLayout.position: SlotsLayout.Leading
                            width: units.gu(4); height: width
                            name: folderListModel.getIcon(model.path)
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
