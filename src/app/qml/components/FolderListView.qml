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
 * Authored by: Arto Jalkanen <ajalkane@gmail.com>
 */
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import org.nemomobile.folderlistmodel 1.0

import "../actions" as FMActions

ScrollView {
    property FolderListModel folderListModel
    property string folderPath: folderListModel.path

    ListView {
        id: root
        anchors.fill: parent
        model: folderListModel
        // This must be visible so Autopilot can see it
        header: SectionDivider {
            objectName: "listViewSmallHeader"
            text: i18n.tr("%1 (%2 file)", "%1 (%2 files)", root.count).arg(folderPath).arg(root.count)
        }

        delegate: FolderListDelegate {
            id: delegate
            leadingActions: ListItemActions {
                actions: [
                    FMActions.Delete {
                        visible: pathIsWritable() //we should discuss that: ((model.filePath.indexOf("/home/phablet/.") === -1) || pageModel.path !== "/home/phablet") && pathIsWritable()
                        onTriggered: {
                            var props = {
                                "folderModel": pageModel,
                                "fileOperationDialog": fileOperationDialog,
                                "filePath" : model.filePath,
                                "fileName" : model.fileName
                            }
                            PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmSingleDeleteDialog.qml"), folderListPage, props)
                        }
                    },

                    FMActions.Rename {
                        visible: pathIsWritable() //we should discuss that: ((model.filePath.indexOf("/home/phablet/.") === -1) || pageModel.path !== "/home/phablet") && pathIsWritable()
                        onTriggered: {
                            var props = { "modelRow"  : model.index, "inputText" : model.fileName, "folderModel": pageModel }
                            PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmRenameDialog.qml"), folderListPage, props)
                        }
                    }
                ]
            }
            trailingActions: ListItemActions {
                actions: [
                    FMActions.ArchiveExtract {
                        visible: getArchiveType(model.fileName) !== ""
                        onTriggered: openFile(model, true)
                    },
                    FMActions.Properties {
                        onTriggered: {
                            var props = { "model": model }
                            PopupUtils.open(Qt.resolvedUrl("../ui/FileDetailsPopover.qml"), folderListPage, props)
                        }
                    },
                    FMActions.FileCut {
                        visible: pathIsWritable() //we should discuss that: ((model.filePath.indexOf("/home/phablet/.") === -1) || pageModel.path !== "/home/phablet") && pathIsWritable()
                        onTriggered: {
                            pageModel.cutIndex(model.index)
                            helpClipboard = true
                        }
                    },
                    FMActions.FileCopy {
                        onTriggered: {
                            pageModel.copyIndex(model.index)
                            helpClipboard = true
                        }
                    },
                    FMActions.Share {
                        visible: !model.isDir
                        onTriggered: openFile(model, true)
                    }
                ]
            }

            onClicked: itemClicked(model)

            onPressAndHold: {
                isContentHub = false
                fileSelectorMode = true
                fileSelector.fileSelectorComponent = pageStack
            }
        }
    }
}
