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
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import Ubuntu.Components.ListItems 1.3

Popover {
    id: root
    objectName: "viewPopover"

    property var folderListModel

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        Standard {
            id: showHiddenFileCheckBox
            objectName: "showHiddenFileCheckBox"

            Label {
                text: i18n.tr("Show Hidden Files")
                anchors.left: parent.left
                anchors.leftMargin: units.gu(2)
                anchors.verticalCenter: parent.verticalCenter
            }

            control: CheckBox {
                anchors.verticalCenter: parent.verticalCenter

                checked: globalSettings.showHidden
                onCheckedChanged: globalSettings.showHidden = checked
            }
        }

        ValueSelector {
            Label {
                text: i18n.tr("View As")
                anchors.left: parent.left
                anchors.leftMargin: units.gu(2)
                anchors.top: parent.top
                anchors.topMargin: units.gu(1.6)
            }

            selectedIndex: globalSettings.viewMethod
            values: [
                i18n.tr("List"),
                i18n.tr("Icons")
            ]

            onSelectedIndexChanged: globalSettings.viewMethod = selectedIndex
        }

        ValueSelector {
            Label {
                text: i18n.tr("Sort By")
                anchors.left: parent.left
                anchors.leftMargin: units.gu(2)
                anchors.top: parent.top
                anchors.topMargin: units.gu(1.6)
            }

            selectedIndex: globalSettings.sortBy
            values: [
                i18n.tr("Name"),
                i18n.tr("Date")
            ]

            onSelectedIndexChanged: globalSettings.sortBy = selectedIndex
        }

        ValueSelector {
            Label {
                text: i18n.tr("Sort Order")
                anchors.left: parent.left
                anchors.leftMargin: units.gu(2)
                anchors.top: parent.top
                anchors.topMargin: units.gu(1.7)
            }

            selectedIndex: globalSettings.sortOrder
            values: [
                i18n.tr("Ascending"),
                i18n.tr("Descending")
            ]

            onSelectedIndexChanged: globalSettings.sortOrder = selectedIndex
        }

        /*Standard {
            Label {
                text: i18n.tr("Filter")
                fontSize: "medium"
                color: UbuntuColors.ash
                anchors.left: parent.left
                anchors.leftMargin: units.gu(2)
                anchors.verticalCenter: parent.verticalCenter
            }

            TextField {
                id: filterField
                anchors {
                    verticalCenter: parent.verticalCenter
                    right: parent.right
                    margins: units.gu(1)
                }

                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText

                text: folderListModel.nameFilters

                onAccepted: goButton.clicked()
                onTextChanged: {
                    if (text !== folderListModel.nameFilters)
                        folderListModel.nameFilters = [text]
                }
            }
        }*/
    }
}
