/*
 * Copyright (C) 2016, 2017 Stefano Verzegnassi
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
import com.ubuntu.filemanager 1.0

import "../backend"

// FIXME: Update model when a folder is removed from the path we have stored

ListView {
    id: rootItem
    anchors { left: parent.left; right: parent.right }
    implicitHeight: units.gu(4)

    rightMargin: rootItem.width * 0.382

    property FolderListModel folderModel

    orientation: ListView.Horizontal
    boundsBehavior: Flickable.StopAtBounds
    clip: true

    delegate: AbstractButton {
        property bool isCurrentFolder: ListView.isCurrentItem
        property bool isLastItem: model.index == (rootItem.count - 1)
        property string name: model.name

        height: rootItem.height
        style: delegateStyle  // Style defined below

        onClicked: folderModel.path = model.path.replace("~", folderModel.model.homePath())
    }

    model: ListModel {
        id: internal
        property string storedPath

        function updateModel() {
            var current_path = folderModel.path.toString()
            var path_slices = []

            var local_prefix = "/"
            var home_prefix = "~"
            var smb_prefix = "smb://"
            var trash_prefix = "trash://"

            current_path = current_path.replace(folderModel.model.homePath(), home_prefix)

            // Check if current path is included in the path we stored previously. This way
            // we still keep a trace of older nodes
            var stored_path = internal.storedPath
            var cur = stored_path.indexOf(current_path)
            if (stored_path && cur == 0) {
                cur += current_path.length
                current_path = current_path + stored_path.slice(cur)
            }

            if (current_path.indexOf(smb_prefix) > -1) {
                current_path = current_path.replace(smb_prefix, "")
                path_slices.push({ name: smb_prefix, path: smb_prefix })
            }

            else if (current_path.indexOf(trash_prefix) > -1) {
                current_path = current_path.replace(trash_prefix, "")
                path_slices.push({ name: trash_prefix, path: trash_prefix })
            }

            else if (current_path.indexOf(home_prefix) > -1) {
                current_path = current_path.replace(home_prefix, "")
                current_path = current_path.slice(1)
                path_slices.push({ name: home_prefix, path: home_prefix })

            }

            else {
                current_path = current_path.slice(1)
                path_slices.push({ name: local_prefix, path: local_prefix })
            }


            var splitted_path = current_path.split("/")

            // Sanitize splitted_path. Remove empty strings from array
            splitted_path = splitted_path.filter(Boolean)

            // Clear the model.
            internal.clear()

            var highlight_index = 0
            for (var i = 0; i < splitted_path.length; ++i) {
                var f = splitted_path[i]

                var objName = f
                var objPath = ""

                if (path_slices[path_slices.length - 1].path != smb_prefix
                        && path_slices[path_slices.length - 1].path != local_prefix) {
                    objPath = path_slices[path_slices.length - 1].path + "/" + f
                } else {
                    objPath = path_slices[path_slices.length - 1].path + f
                }

                path_slices.push({ name: objName, path: objPath })

                // Set the current item, if necessary
                if (objPath == folderModel.path.toString().replace(folderModel.model.homePath(), home_prefix)) {
                    highlight_index = i + 1
                }
            }

            internal.append(path_slices)
            rootItem.currentIndex = highlight_index
            internal.storedPath = path_slices[path_slices.length - 1].path
        }
    }

    // Ensure the currentItem is always visible
    onCurrentIndexChanged: {
        positionViewTimerWorkaround.restart()
    }

    Timer {
        id: positionViewTimerWorkaround

        function __gotoIndex(i) {
            if (currentItem.x > rootItem.contentX &&
                    currentItem.x + currentItem.height < rootItem.contentX + rootItem.width)
                return;

            var pX = rootItem.contentX
            var dpX
            rootItem.positionViewAtIndex(i, ListView.Center)
            dpX = rootItem.contentX
            positionAnimation.from = pX
            positionAnimation.to = dpX
            positionAnimation.running = true
        }

        interval: 1
        onTriggered: __gotoIndex(currentIndex)

        property QtObject positionAnimation: UbuntuNumberAnimation {
            target: rootItem
            property: "contentX"
            duration: UbuntuAnimation.SnapDuration
        }
    }

    // Subscribe to folderModel 'folder' changes
    Component.onCompleted: internal.updateModel()
    Connections {
        target: folderModel
        onPathChanged: internal.updateModel()
    }

    /*
     * Scroll helpers
     * On desktop, or when a mouse is connected to the device, we may want to make easier
     * for the user to scroll the path bar.
     * For that reason, we add a MouseArea which automatically scrolls the content when
     * the mouse cursor hovers rootItem's boundaries.
     */

    MouseArea {
        id: scrollLeftArea
        width: units.gu(4)
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }

        // TODO: Uncomment when UITK will support this property
        enabled: rootItem.contentX != 0 && (rootItem.contentWidth > rootItem.width)  // && QuickUtils.mouseAttached
        visible: enabled  // Don't steal mouse event when not enabled

        hoverEnabled: true
        SmoothedAnimation {
            target: rootItem
            property: "contentX"
            to: 0
            velocity: units.gu(30)
            running: scrollLeftArea.containsMouse
        }
    }

    MouseArea {
        id: scrollRightArea
        width: units.gu(4)
        anchors {
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }

        // TODO: Uncomment when UITK will support this property
        enabled: rootItem.contentX != parseInt(rootItem.contentWidth - rootItem.width) && (rootItem.contentWidth > rootItem.width)  // && QuickUtils.mouseAttached
        visible: enabled  // Don't steal mouse event when not enabled

        hoverEnabled: true
        SmoothedAnimation {
            target: rootItem
            property: "contentX"
            to: contentWidth - width
            velocity: units.gu(30)
            running: scrollRightArea.containsMouse
        }
    }

    /*
     * Delegate style
     * AbstractButton inherits StyledItem. This allows us to keep logic and style
     * separated, improving the readability of the code.
     */

    Rectangle {
        anchors.fill: parent
        color: theme.palette.normal.base
        z: -1000
    }

    Component {
        id: delegateStyle
        Item {
            property color  inactiveColor: theme.palette.normal.backgroundText
            property color  activeColor: theme.palette.selected.positionText

            property int    labelMaximumWidth: units.gu(24)
            property int    labelTextSize: Label.Small  /*Medium*/

            property string dividerIconName: "go-next"
            property int    dividerIconSize: units.gu(1.5)

            implicitWidth: delegateRow.width + delegateRow.spacing

            PathArrowBackground {
                anchors.fill: parent
                anchors.margins: units.dp(1)
                anchors.leftMargin: units.dp(1) - arrowWidth
                arrowWidth: units.gu(1)
                color: styledItem.pressed ? theme.palette.highlighted.background
                                          : styledItem.hovered ? theme.palette.selected.background : theme.palette.normal.background
            }

            Row {
                id: delegateRow
                anchors.verticalCenter: parent.verticalCenter
                //spacing: units.gu(0.5)

                property bool isHomePath: styledItem.name == "~"
                property bool isRootPath: styledItem.name == "/"
                property bool isTrashPath: styledItem.name == "trash://"
                property bool isSmbPath: styledItem.name == "smb://"

                Item {
                    height: parent.height
                    width: units.gu(2.25)
                    /* SPACER */
                }

                Label {
                    anchors.verticalCenter: parent.verticalCenter
                    width: visible ? Math.min(implicitWidth, labelMaximumWidth) : 0
                    textSize: labelTextSize
                    elide: Text.ElideMiddle
                    text: delIcon.visible ? "" : styledItem.name
                    color: styledItem.isCurrentFolder ? activeColor : inactiveColor
                    font.weight: styledItem.hovered ? Font.Normal : Font.Light
                }

                Icon {
                    id: delIcon
                    anchors.verticalCenter: parent.verticalCenter
                    width: visible ? units.gu(2) : 0
                    height: width
                    name: delegateRow.isRootPath
                          ? "computer-symbolic"
                          : delegateRow.isHomePath ? "go-home"
                                                   : delegateRow.isSmbPath ? "network-vpn" : "delete"
                    color: styledItem.isCurrentFolder ? activeColor : inactiveColor
                    visible: delegateRow.isRootPath || delegateRow.isHomePath || delegateRow.isTrashPath || delegateRow.isSmbPath
                }

                Item {
                    height: parent.height
                    width: units.gu(3.25)
                    /* SPACER */
                }
            }
        }
    }

    /*
     * Fade out ListView at boundaries.
     * This is enabled only when if content is wider than the container, in
     * order to suggest that it can be flipped.
     * Also this works as hint for scrolling by mouse hovering.
     *
     * ref. http://stackoverflow.com/questions/13138868/qt-qml-fade-image-horizontally-i-e-from-left-to-right-not-whole-image
     */

    layer.enabled: true
    layer.effect: ShaderEffect {
        property int fadeOutEffectWidth: units.gu(4)
        property real fadeOutEffectRelativeWidth: fadeOutEffectWidth / rootItem.width

        property bool leftFadeOutEffectActive: scrollLeftArea.enabled
        property bool rightFadeOutEffectActive: scrollRightArea.enabled

        fragmentShader: "
            varying highp vec2 qt_TexCoord0;
            uniform lowp sampler2D source;

            // Properties defined above
            uniform lowp float fadeOutEffectRelativeWidth;
            uniform bool leftFadeOutEffectActive;
            uniform bool rightFadeOutEffectActive;

            void main(void)
            {
                highp vec4 texture = texture2D(source, qt_TexCoord0);
                lowp float alpha = 1.0;

                if (leftFadeOutEffectActive && (qt_TexCoord0.x < fadeOutEffectRelativeWidth))
                    alpha = 0.25 + (qt_TexCoord0.x / fadeOutEffectRelativeWidth) * 0.75;

                if (rightFadeOutEffectActive && (qt_TexCoord0.x > (1.0 - fadeOutEffectRelativeWidth)))
                    alpha = 0.25 + (1.0 - qt_TexCoord0.x) / fadeOutEffectRelativeWidth * 0.75;

                gl_FragColor = texture * alpha;
            }
        "
    }
}
