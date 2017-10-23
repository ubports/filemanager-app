/*
 * Copyright (C) 2017 Stefano Verzegnassi
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
 */

import QtQuick 2.4
import Ubuntu.Components 1.3

ListItem {
    id: rootItem
    height: layout.height

    property int selectedIndex: -1
    property var model

    property string title
    property string subtitle
    property string summary

    ListItemLayout {
        id: layout

        anchors { left: parent.left; right: parent.right }
        anchors.leftMargin: units.gu(-2)
        anchors.rightMargin: units.gu(-2)

        title.text: rootItem.title
        title.textSize: Label.Medium
        subtitle.text: rootItem.subtitle
        subtitle.textSize: Label.Medium
        summary.text: rootItem.summary
        summary.textSize: Label.Medium

        Row {
            id: choicesRow
            SlotsLayout.position: SlotsLayout.Last
            height: units.gu(4)

            Repeater {
                model: rootItem.model

                delegate: AbstractButton {
                    id: del
                    property bool isSelected: model.index == rootItem.selectedIndex

                    onClicked: rootItem.selectedIndex = model.index

                    width: delLabel.width + units.gu(2)
                    height: parent.height

                    Rectangle {
                        anchors.fill: parent
                        color: "#CDCDCD"
                        visible: del.pressed
                    }

                    Label {
                        id: delLabel
                        anchors.centerIn: parent
                        text: modelData
                        textSize: Label.Medium
                        color: isSelected ? theme.palette.normal.backgroundText : theme.palette.disabled.backgroundText
                    }
                }
            }
        }
    }
}
