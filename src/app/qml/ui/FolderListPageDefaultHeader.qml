import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../components" as Components
import "../actions" as FMActions

PageHeader {
    id: rootItem

    // temp
    property var fileOperationDialog
    property var folderModel

    title: folderModel.basename(folderModel.path)

    // FIXME: Disabled for now as it's broken, to replace with new one
    //contents: Components.PathHistoryRow {}

    leadingActionBar.actions: FMActions.GoBack {
        onTriggered: folderModel.goBack()
    }

    trailingActionBar.numberOfSlots: 3
    trailingActionBar.actions: [
        FMActions.FilePaste {
            clipboardUrlsCounter: folderModel.model.clipboardUrlsCounter
            visible: folderModel.model.clipboardUrlsCounter > 0
            onTriggered: {
                console.log("Pasting to current folder items of count " + folderModel.model.clipboardUrlsCounter)
                fileOperationDialog.startOperation(i18n.tr("Paste files"))
                folderModel.model.paste()
            }
        },

        FMActions.FileClearSelection {
            clipboardUrlsCounter: folderModel.model.clipboardUrlsCounter
            visible: folderModel.model.clipboardUrlsCounter > 0
            onTriggered: {
                console.log("Clearing clipboard")
                // FIXME: Seems not to clear actually
                folderModel.model.clearClipboard()
            }
        },

        FMActions.Settings {
            onTriggered: PopupUtils.open(Qt.resolvedUrl("ViewPopover.qml"), parent, { folderListModel: folderModel.model })
        },

        FMActions.NewFolder {
            visible: folderModel.model.isWritable
            onTriggered: {
                print(text)
                PopupUtils.open(Qt.resolvedUrl("../dialogs/CreateFolderDialog.qml"), mainView, { folderModel: folderModel.model })
            }
        },

        FMActions.Properties {
            onTriggered: {
                print(text)
                PopupUtils.open(Qt.resolvedUrl("FileDetailsPopover.qml"), mainView,{ "model": folderModel.model })
            }
        },

        FMActions.UnlockFullAccess {
            visible: folderModel.model.onlyAllowedPaths
            onTriggered: {
                console.log("Full access clicked")
                var authDialog = PopupUtils.open(Qt.resolvedUrl("../dialogs/AuthenticationDialog.qml"), mainView)

                authDialog.passwordEntered.connect(function(password) {
                    if (pamAuthentication.validatePasswordToken(password)) {
                        console.log("Authenticated for full access")
                        mainView.fullAccessGranted = true
                    } else {
                        var props = { title: i18n.tr("Authentication failed") }
                        PopupUtils.open(Qt.resolvedUrl("../dialogs/NotifyDialog.qml"), mainView, props)

                        console.log("Could not authenticate")
                    }
                })
            }
        }
    ]
}

