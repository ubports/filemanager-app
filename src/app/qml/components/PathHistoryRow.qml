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
 * Authored by: Akiva
 */
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.ListItems 1.3

/* Full path of your current folder and recent history, that you can jump to by clicking its members */


Flickable {
    id: flickable
    anchors {
        fill: parent
        topMargin: units.gu(1)
        rightMargin: units.gu(1)
        bottomMargin: units.gu(1)
    }
    /* Convenience properties ; used a large amount of times to warrant a variable */
    property int iconWidth: units.gu(2.5)
    property string textSize: "large"
    property string separatorText: " /"


    /* contentWidth equals this to allow it to hide Device and Home */
    contentWidth: {
        repeater.model > 0 ?
                    memoryRepeater.model > 0 ?
                        width + row.width - memoryRepeater.itemAt(memoryRepeater.model-1).width + memoryRow.width
                      : width + row.width - repeater.itemAt(repeater.model-1).width
        : width + memoryRow.width - memoryRepeater.itemAt(memoryRepeater.model-1).width
    }
    anchors {
        fill: parent
    }
    clip: true
    boundsBehavior: Flickable.StopAtBounds

    Behavior on contentX { SmoothedAnimation { duration: 555 }}

    /* This prevents the contentWidth from causing sudden flicks */
    Behavior on contentWidth { SmoothedAnimation { duration: 500 }}

    /* Flickable Contents */
    Row {
        id: row
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        spacing: 0 // Safety; having any spacing will throw off the contentX calculations.

        function repositionScrollable() {
            if (repeater.count === 0) {
                flickable.contentX = 0;
            } else {
                flickable.contentX = row.width - repeater.itemAt(repeater.model - 1).width
            }
        }
        /* Adjust contentX according to the current folder */
        onWidthChanged: {
            repositionScrollable()
        }

        /* Root Folder displayed as "Device" */
        AbstractButton {
            id: device
            width: deviceLabel.width + flickable.iconWidth
            height: parent.height
            onClicked: goTo("/")

            Label {
                id: deviceLabel
                text: i18n.tr("Device")
                fontSize: flickable.textSize
                anchors.verticalCenter: parent.verticalCenter
                color: UbuntuColors.inkstone
                clip: true
                /* Maximum Width = Flickable Width */
                width: if (contentWidth > flickable.width) { flickable.width }
            }
        }

        /* Current Directory and its parents */
        Repeater {
            id: repeater

            model: pathModel(folder)
            property int memoryModel: memoryModel ? memoryModel : pathModel(userplaces.locationHome)
            property string memoryPath: memoryPath ? memoryPath : userplaces.locationHome

            /* Memory Management */
            onModelChanged: {
                /* Extend Memory */
                if (model > memoryModel && memoryPath === pathRaw(folder, memoryModel-1)) {
                    memoryModel = model
                    memoryPath = folder
                    // console.log("/* Extend Memory */")
                    // console.log("model > memoryModel && memoryPath === pathRaw(folder, memoryModel-1")
                }
                /* Reset Memory to Current */
                else if (folder !== pathRaw(memoryPath,model-1) && model > 0) {
                    memoryModel = pathModel(folder)
                    memoryPath = folder
                    // console.log("/* Reset Memory to Current */")
                    // console.log("folder !== pathRaw(memoryPath,model")
                }
                // console.log("Repeat Model = " + repeater.model)
                // console.log("Current Path = " + folder)
                // console.log("Memory Model = " + memoryModel)
                // console.log("Memory Path  = " + memoryPath)
            }

            delegate: AbstractButton {
                visible: folder !== "/" // This is to avoid issues with naming the root folder, "Device"
                width: label.width + pathSeparator.width
                height: row.height
                onClicked: {
                    // When clicking on an already selected item, go up one level. Otherwise go to
                    // the clicked item. This behaviour is to make it easy to go up in the folder
                    // hierarchy now that the "back" button goes back in history and not up the directory
                    // hierarchy
                    if (repeater.model === index + 1) {
                        goUp()
                    } else {
                        goTo(pathRaw(folder, index))
                    }
                }

                Label {
                    id: label
                    text: pathText(folder,index)
                    fontSize: flickable.textSize
                    anchors.verticalCenter: parent.verticalCenter
                    color: UbuntuColors.inkstone
                    opacity: repeater.model === index + 1 ? 1.0 : 0.3
                    clip: true

                    /* Maximum Width = Flickable Width */
                    width: if (contentWidth > flickable.width) { flickable.width } else { contentWidth }
                }

                Label {
                    id: pathSeparator
                    text: separatorText
                    fontSize: flickable.textSize
                    width: flickable.iconWidth
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: label.left
                    color: UbuntuColors.inkstone
                    opacity: label.opacity
                    // clip: true
                }
            }
        }
    }

    /* Memory of Previously visited folders */
    Row {
        id: memoryRow
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: row.right // Not placed in the other row, to help avoid making contentX calculations more complicated.
        }
        /* Previously visited folders */
        Repeater {
            id: memoryRepeater
            model: repeater.memoryModel - repeater.model

            delegate: AbstractButton {
                width: memoryLabel.width + memoryPathSeparator.width
                height: memoryRow.height
                onClicked: goTo(pathRaw(repeater.memoryPath, repeater.memoryModel-memoryRepeater.model+index))

                Label {
                    id: memoryLabel
                    text: repeater.model > 0 ? pathText(repeater.memoryPath,repeater.memoryModel-memoryRepeater.model+index)
                                             : pathText(repeater.memoryPath,index)
                    fontSize: flickable.textSize
                    anchors.verticalCenter: parent.verticalCenter
                    color: UbuntuColors.inkstone
                    opacity: 0.3
                    clip: true

                    /* Maximum Width = Flickable Width */
                    width: if (contentWidth > flickable.width) { flickable.width } else { contentWidth }
                }

                Label {
                    id: memoryPathSeparator
                    text: separatorText
                    fontSize: flickable.textSize
                    width: flickable.iconWidth
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: memoryLabel.left
                    color: UbuntuColors.inkstone
                    opacity: 0.3
                    // clip: true
                }
            }
        }
    }
}

