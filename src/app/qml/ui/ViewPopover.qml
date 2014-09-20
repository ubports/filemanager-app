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
import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0

Popover {
    id: root
    objectName: "viewPopover"

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
                fontSize: "medium"
                color: Theme.palette.normal.overlayText
                anchors.left: parent.left
                anchors.leftMargin: units.gu(2)
                anchors.verticalCenter: parent.verticalCenter
            }

            control: CheckBox {
                anchors.verticalCenter: parent.verticalCenter

                checked: fileView.showHiddenFiles
                onCheckedChanged: {
                    fileView.showHiddenFiles = checked
                }
            }
        }

        ValueSelector {
            Label {
                text: i18n.tr("View As")
                fontSize: "medium"
                color: Theme.palette.normal.overlayText
                anchors.left: parent.left
                anchors.leftMargin: units.gu(2)
                anchors.top: parent.top
                anchors.topMargin: units.gu(1.6)
            }

            selectedIndex: values.indexOf(viewMethod)
            values: [
                i18n.tr("Icons"),
                i18n.tr("List")
            ]

            onSelectedIndexChanged: {
                saveSetting("viewMethod", values[selectedIndex])
            }
        }

        ValueSelector {
            Label {
                text: i18n.tr("Sort By")
                fontSize: "medium"
                color: Theme.palette.normal.overlayText
                anchors.left: parent.left
                anchors.leftMargin: units.gu(2)
                anchors.top: parent.top
                anchors.topMargin: units.gu(1.6)
            }

            selectedIndex: values.indexOf(fileView.sortingMethod)
            values: [
                i18n.tr("Name"),
                i18n.tr("Date")
            ]

            onSelectedIndexChanged: {
                fileView.sortingMethod = values[selectedIndex]
            }
        }

        ValueSelector {
            Label {
                text: i18n.tr("Sort Order")
                fontSize: "medium"
                color: Theme.palette.normal.overlayText
                anchors.left: parent.left
                anchors.leftMargin: units.gu(2)
                anchors.top: parent.top
                anchors.topMargin: units.gu(1.7)
            }

            selectedIndex: sortAscending ? 0 : 1
            values: [
                i18n.tr("Ascending"),
                i18n.tr("Descending")
            ]

            onSelectedIndexChanged: {
                fileView.sortAscending = (values[selectedIndex] === i18n.tr("Ascending"))
            }
        }

        Standard {
            visible: showAdvancedFeatures

            Label {
                text: i18n.tr("Filter")
                fontSize: "medium"
                color: Theme.palette.normal.overlayText
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

                inputMethodHints: Qt.ImhNoAutoUppercase

                text: pageModel.nameFilters

                onAccepted: goButton.clicked()
                onTextChanged: {
                    if (text !== pageModel.nameFilters)
                        pageModel.nameFilters = [text]
                }
            }
        }
    }
}
