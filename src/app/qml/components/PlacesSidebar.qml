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
import QtGraphicalEffects 1.0
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0
import Ubuntu.Components.Popups 1.0

Sidebar {
    id: root

    //color: Qt.rgba(0.5,0.5,0.5,0.3)
    width: expanded ? (collapsed ? units.gu(8) : units.gu(22)) : 0

    property bool collapsed: collapsedSidebar

    MouseArea {
        anchors.fill: parent
        onClicked: {
            saveSetting("collapsedSidebar", !collapsedSidebar)
        }
    }

    property bool tempExpanded: false

    Behavior on width {
        UbuntuNumberAnimation {}
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

            model: userplaces

            delegate: Standard {
                objectName: "place" + folderDisplayName(path).replace(/ /g,'')
                text: folderDisplayName(path)

                Image {
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                    }

                    height: units.gu(1.1)
                    width: height

                    source: Qt.resolvedUrl("../icons/arrow.png")
                    opacity: selected && collapsed ? 1 : 0

                    Behavior on opacity {
                        UbuntuNumberAnimation {}
                    }
                }

                iconSource: model.icon || fileIcon(model.path, true)

                onClicked: {
                    goTo(model.path)
                }

                height: units.gu(5)
                showDivider: !collapsed

                // This refers to a parent FolderListPage.folder
                selected: folder === path
                iconFrame: false
            }
        }
    }
}
