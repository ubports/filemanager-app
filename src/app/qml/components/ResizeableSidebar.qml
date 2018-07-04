/*
 * Copyright (C) 2015, 2016 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
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

Item {
    id: resizeableSidebar

    /* Remember to set left or right anchor */
    anchors { top: parent.top; bottom: parent.bottom }

    property int minimumWidth: units.gu(20)
    property int maximumWidth: units.gu(40)

    readonly property bool resizing: resizerSensing.pressed

    Rectangle {
        id: verticalDivider
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: resizeableSidebar.anchors.left ? parent.right : undefined
            left: resizeableSidebar.anchors.left ? undefined : parent.left
        }
        width: units.dp(1)
        color: theme.palette.normal.base

        // Stay above the sidebar content
        z: 10

        MouseArea {
            id: resizerSensing
            objectName: "Divider"
            enabled: verticalDivider.width > 0
            anchors {
                fill: parent
                leftMargin: enabled ? -units.gu(1) : 0
                rightMargin: enabled ? -units.gu(1) : 0
            }
            cursorShape: Qt.SizeHorCursor
            drag {
                axis: Drag.XAxis
                target: resizer
                smoothed: false
                minimumX: resizeableSidebar.minimumWidth
                maximumX: resizeableSidebar.maximumWidth
            }
            onPressed: resizer.x = resizeableSidebar.width
        }
        states: State {
            name: "active"
            when: resizerSensing.pressed || resizerSensing.containsMouse
            PropertyChanges {
                target: verticalDivider
                color: Qt.darker(theme.palette.normal.background, 1.5)
            }
        }
        transitions: Transition {
            from: ""
            to: "*"
            reversible: true
            ColorAnimation {
                target: verticalDivider
                property: "color"
                duration: UbuntuAnimation.SlowDuration
            }
        }
    }
    Item {
        id: resizer
        height: parent.height
        onXChanged: resizeableSidebar.width = x
    }

    VerticalDivider {
        anchors {
            top: parent.top
            bottom: parent.bottom
            right: parent.right
        }
    }
}
