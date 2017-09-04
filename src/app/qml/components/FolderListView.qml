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
import Ubuntu.Components.ListItems 1.3 as OldListItems
import Ubuntu.Components.Popups 1.3
import org.nemomobile.folderlistmodel 1.0

Item {
    property FolderListModel folderListModel
    property string folderPath: folderListModel.path
    property bool smallMode: !wideAspect
    property Flickable flickable: root
    ListView {
        id: root
        anchors.fill: parent
        model: folderListModel
        // This must be visible so Autopilot can see it
        header: OldListItems.Header {
            objectName: "listViewSmallHeader"
            text: i18n.tr("%1 (%2 file)", "%1 (%2 files)", root.count).arg(folderPath).arg(root.count)
            height: smallMode ? units.gu(4) : 0
            clip: true
        }

        delegate: FolderListDelegate {
            id: delegate
            leadingActions: ListItemActions {
                actions: [
                    Action {
                        iconName: "edit-delete"
                        visible: (model.filePath.indexOf("/home/phablet/.") === -1) && pathIsWritable()
                        onTriggered: {
                            PopupUtils.open(confirmSingleDeleteDialog, folderListPage,
                                            { "filePath" : model.filePath,
                                                "fileName" : model.fileName }
                                            )
                        }
                    },
                    Action {
                        iconName: "edit"
                        visible: (model.filePath.indexOf("/home/phablet/.") === -1) && pathIsWritable()
                        onTriggered: {
                            PopupUtils.open(confirmRenameDialog, folderListPage,
                                            { "modelRow"  : model.index,
                                                "inputText" : model.fileName
                                            })
                        }
                    }
                ]
            }
            trailingActions: ListItemActions {
                actions: [
                    Action {
                        iconName: "application-x-archive-symbolic"
                        visible: getArchiveType(model.fileName) !== ""
                        onTriggered: {
                            openFile(model, true)
                        }
                    },
                    Action {
                        iconName: "info"
                        onTriggered: {
                            PopupUtils.open(Qt.resolvedUrl("../ui/FileDetailsPopover.qml"),
                                            folderListPage,
                                            { "model": model
                                            }
                                            )
                        }
                    },
                    Action {
                        iconName: "edit-cut"
                        onTriggered: {
                            pageModel.cutIndex(model.index)
                            helpClipboard = true
                        }
                    },
                    Action {
                        iconName: "edit-copy"
                        onTriggered: {
                            pageModel.copyIndex(model.index)
                            helpClipboard = true
                        }
                    },
                    Action {
                        iconName: "share"
                        visible: !model.isDir
                        onTriggered: {
                            openFile(model, true)
                        }
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
    Scrollbar {
        flickableItem: root
        align: Qt.AlignTrailing
    }
}
