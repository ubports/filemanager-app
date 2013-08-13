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
    objectName: "settingsPopover"

    Column {
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }

        Standard {
            id: showHiddenFileCheckBox
            objectName: "showHiddenFileCheckBox"

            text: i18n.tr("Show Hidden Files")
            control: CheckBox {
                anchors.verticalCenter: parent.verticalCenter

                checked: fileView.showHiddenFiles
                onCheckedChanged: {
                    fileView.showHiddenFiles = checked
                }
            }
        }

        ValueSelector {
            text: i18n.tr("Sort By")
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
            text: i18n.tr("Sort Order")
            selectedIndex: sortAccending ? 0 : 1
            values: [
                i18n.tr("Ascending"),
                i18n.tr("Descending")
            ]

            onSelectedIndexChanged: {
                fileView.sortAccending = (values[selectedIndex] === i18n.tr("Ascending"))
            }
        }

        Standard {
            visible: showAdvancedFeatures
            text: i18n.tr("Filter")

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
