import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../components" as Components
import "../actions" as FMActions

PageHeader {
    id: rootItem

    // temp
    property var folderListPage
    property var fileOperationDialog
    property var pageModel

    title: folderListPage.basename(folder)
    contents: Components.PathHistoryRow {}

    leadingActionBar.actions: FMActions.GoBack {
        onTriggered: goBack()
    }

    trailingActionBar.numberOfSlots: 3
    trailingActionBar.actions: [
        FMActions.FilePaste {
            clipboardUrlsCounter: pageModel.clipboardUrlsCounter
            visible: folderListPage.helpClipboard // pageModel.clipboardUrlsCounter > 0
            onTriggered: {
                console.log("Pasting to current folder items of count " + pageModel.clipboardUrlsCounter)
                fileOperationDialog.startOperation(i18n.tr("Paste files"))
                pageModel.paste()
            }
        },

        FMActions.FileClearSelection {
            clipboardUrlsCounter: pageModel.clipboardUrlsCounter
            visible: folderListPage.helpClipboard // pageModel.clipboardUrlsCounter > 0
            onTriggered: {
                console.log("Clearing clipboard")
                pageModel.clearClipboard()
                folderListPage.helpClipboard = false
            }
        },

        FMActions.Settings {
            onTriggered: PopupUtils.open(Qt.resolvedUrl("ViewPopover.qml"), parent, { folderListModel: pageModel })
        },

        FMActions.NewFolder {
            visible: folderListPage.__pathIsWritable
            onTriggered: {
                print(text)
                PopupUtils.open(Qt.resolvedUrl("../dialogs/CreateFolderDialog.qml"), folderListPage, { folderModel: pageModel })
            }
        },

        FMActions.Properties {
            onTriggered: {
                print(text)
                PopupUtils.open(Qt.resolvedUrl("FileDetailsPopover.qml"), folderListPage,{ "model": pageModel})
            }
        },

        FMActions.UnlockFullAccess {
            visible: pageModel.onlyAllowedPaths
            onTriggered: {
                console.log("Full access clicked")
                var authDialog = PopupUtils.open(Qt.resolvedUrl("../dialogs/AuthenticationDialog.qml"), folderListPage)

                authDialog.passwordEntered.connect(function(password) {
                    if (pamAuthentication.validatePasswordToken(password)) {
                        console.log("Authenticated for full access")
                        mainView.fullAccessGranted = true
                    } else {
                        var props = { title: i18n.tr("Authentication failed") }
                        PopupUtils.open(Qt.resolvedUrl("../dialogs/NotifyDialog.qml"), folderListPage, props)

                        console.log("Could not authenticate")
                    }
                })
            }
        }
    ]
}

