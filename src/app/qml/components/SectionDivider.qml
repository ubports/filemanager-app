/*
 * Copyright (C) 2017 Stefano Verzegnassi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License 3 as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import QtQuick.Layouts 1.1

Item {
    property alias text: label.text

    anchors { left: parent.left; right: parent.right }
    height: units.gu(4)

    RowLayout {
        anchors {
            left: parent.left;
            right: parent.right
            verticalCenter: parent.bottom
        }

        spacing: units.gu(2)

        Rectangle {
            height: units.dp(1)
            color: theme.palette.normal.base
            Layout.preferredWidth: units.gu(7)
            Layout.alignment: Qt.AlignVCenter
        }

        Label {
            id: label
            Layout.alignment: Qt.AlignVCenter
            textSize: Label.Small
            color: theme.palette.normal.backgroundSecondaryText
        }

        Rectangle {
            height: units.dp(1)
            color: theme.palette.normal.base
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
        }
    }
}
