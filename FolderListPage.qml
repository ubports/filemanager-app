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
import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import org.nemomobile.folderlistmodel 1.0

Page {
    id: root
    anchors.fill: parent

    title: folderName(folder)

    property bool showHiddenFiles: false

    property variant fileView: root

    onShowHiddenFilesChanged: {
        pageModel.showHiddenFiles = root.showHiddenFiles
    }

    // This stores the location using ~ to represent home
    property string folder
    property string homeFolder: "~"

    // This replaces ~ with the actual home folder, since the
    // plugin doesn't recognize the ~
    property string path: folder.replace("~", pageModel.homePath())

    function goHome() {
        goTo(root.homeFolder)
    }

    function goTo(location) {
        // Since the FolderListModel returns paths using the actual
        // home folder, this replaces with ~ before actually going
        // to the specified folder
        root.folder = location.replace(pageModel.homePath(), "~")
    }

    function folderName(folder) {
        if (folder === root.homeFolder) {
            return i18n.tr("Home")
        } else if (folder === "/") {
            return i18n.tr("File System")
        } else {
            return folder.substr(folder.lastIndexOf('/') + 1)
        }
    }

    FolderListModel {
        id: pageModel

        path: root.path
    }

    ActionSelectionPopover {
        id: folderActionsPopover
        objectName: "folderActionsPopover"

        actions: ActionList {
            Action {
                text: i18n.tr("Create new folder")
                onTriggered: {
                    print(text)

                    PopupUtils.open(createFolderDialog, root)
                }
            }

            // TODO: Disabled until backend supports creating files
//            Action {
//                text: i18n.tr("Create new file")
//                onTriggered: {
//                    print(text)

//                    PopupUtils.open(createFileDialog, root)
//                }
//            }

            Action {
                text: pageModel.clipboardUrlsCounter === 0
                      ? i18n.tr("Paste")
                      : pageModel.clipboardUrlsCounter === 1
                        ? i18n.tr("Paste %1 file").arg(pageModel.clipboardUrlsCounter)
                        : i18n.tr("Paste %1 files").arg(pageModel.clipboardUrlsCounter)
                onTriggered: {
                    console.log("Pasting to current folder items of count " + pageModel.clipboardUrlsCounter)
                    PopupUtils.open(Qt.resolvedUrl("FileOperationProgressDialog.qml"),
                                    root,
                                    {
                                        title: i18n.tr("Paste files"),
                                        folderListModel: pageModel
                                     }
                                    )


                    pageModel.paste()
                }

                // FIXME: This property is depreciated and doesn't seem to work!
                //visible: pageModel.clipboardUrlsCounter > 0

                enabled: pageModel.clipboardUrlsCounter > 0
            }

            // FIXME: Doesn't work!
//            Action {
//                text: i18n.tr("Properties")
//                onTriggered: {
//                    print(text)
//                    PopupUtils.open(Qt.resolvedUrl("FileDetailsPopover.qml"),
//                                    root,
//                                        { "model": pageModel
//                                        }
//                                    )
//                }
//            }
        }

        // Without this the popover jumps up at the start of the application. SDK bug?
        // Bug report has been made of these https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1152270
        visible: false
    }

    Component {
        id: createFolderDialog
        ConfirmDialogWithInput {
            title: i18n.tr("Create folder")
            text: i18n.tr("Enter name for new folder")

            onAccepted: {
                console.log("Create folder accepted", inputText)
                if (inputText !== '') {
                    pageModel.mkdir(inputText)
                } else {
                    console.log("Empty directory name, ignored")
                }
            }
        }
    }

    Component {
        id: createFileDialog
        ConfirmDialogWithInput {
            title: i18n.tr("Create file")
            text: i18n.tr("Enter name for new file")

            onAccepted: {
                console.log("Create file accepted", inputText)
                if (inputText !== '') {
                    //FIXME: Actually create a new file!
                } else {
                    console.log("Empty file name, ignored")
                }
            }
        }
    }



    tools: ToolbarItems {
        id: toolbar
        locked: true
        opened: true

        back: ToolbarButton {
            text: "Up"
            iconSource: "/usr/share/icons/ubuntu-mobile/actions/scalable/keyboard-caps.svg"
            visible: folder != "/"
            onTriggered: {
                goTo(pageModel.parentPath)
            }
        }

        ToolbarButton {
            text: i18n.tr("Actions")
            iconSource: "/usr/share/icons/ubuntu-mobile/actions/scalable/edit.svg"

            onTriggered: {
                print(text)
                folderActionsPopover.caller = caller
                folderActionsPopover.show();
            }
        }

        ToolbarButton {
            text: i18n.tr("Settings")
            iconSource: "/usr/share/icons/ubuntu-mobile/actions/scalable/settings.svg"

            onTriggered: {
                print(text)

                PopupUtils.open(Qt.resolvedUrl("SettingsPopover.qml"), caller)
            }
        }

        ToolbarButton {
            text: i18n.tr("Places")
            iconSource: "/usr/share/icons/ubuntu-mobile/actions/scalable/location.svg"
            onTriggered: {
                print(text)

                PopupUtils.open(Qt.resolvedUrl("PlacesPopover.qml"), caller)
            }
        }
    }

    Column {
        anchors.centerIn: root
        Label {
            text: i18n.tr("No files")
            fontSize: "large"
            visible: folderListView.count == 0 && !pageModel.awaitingResults
        }
        ActivityIndicator {
            running: pageModel.awaitingResults
            width: units.gu(8)
            height: units.gu(8)
        }
    }

    FolderListView {
        id: folderListView

        clip: true

        folderListModel: pageModel
        anchors.fill: parent
        // IMPROVE: this should work (?), but it doesn't. Height is undefined. Anyway in previous
        // SDK version the parent size was properly initialized. Now the size of toolbar is not taken into
        // account and apparently you can't even query toolbar's height.
     // anchors.bottomMargin: toolbar.height
        // So hard-code it. Not nice at all:
        anchors.bottomMargin: units.gu(8)
    }

    // Errors from model
    Connections {
        target: pageModel
        onError: {
            console.log("FolderListModel Error Title/Description", errorTitle, errorMessage)
            PopupUtils.open(Qt.resolvedUrl("NotifyDialog.qml"), root,
                            {
                                // Unfortunately title can not handle too long texts. TODO: bug report
                                // title: i18n.tr(errorTitle),
                                title: i18n.tr("File operation error"),
                                text: errorTitle + ": " + errorMessage
                            })
        }
    }
}
