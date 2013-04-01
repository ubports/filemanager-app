import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import org.nemomobile.folderlistmodel 1.0

ListView {
    id: root

    property FolderListModel folderListModel
    property string path: folderListModel.path
    model: folderListModel

    Component {
        id: confirmSingleDeleteDialog
        ConfirmDialog {
            property string filePath
            property string fileName
            title: i18n.tr("Delete?")
            text: "Are you sure you want to permanently delete '" + fileName + "'?"

            onAccepted: {
                console.log("Delete accepted for filePath, fileName", filePath, fileName)

                folderListModel.rm(filePath)
            }
        }
    }

    ActionSelectionPopover {
        id: actionSelectionPopover
        property var model
        actions: ActionList {
            Action {
                text: i18n.tr("Add as bookmark")
                // TODO
                onTriggered: print(text)
            }
            Action {
                text: i18n.tr("Show details")
                onTriggered: {
                    print(text)
                    PopupUtils.open(Qt.resolvedUrl("FileDetailsPopover.qml"),
                                    actionSelectionPopover.caller,
                                        { "model": actionSelectionPopover.model
                                        }
                                    )
                }
            }
            Action {
                text: i18n.tr("Delete")
                onTriggered: {
                    print(text)
                    PopupUtils.open(confirmSingleDeleteDialog, actionSelectionPopover.caller,
                                    { "filePath" : actionSelectionPopover.model.filePath,
                                      "fileName" : actionSelectionPopover.model.fileName }
                                    )
                }
            }
            
        }
        // TODO: problem: clicking outside popup makes the click go through to the
        // folder listview, so for example you'd change directory while only trying
        // to dismiss the popup. Maybe SDK bug, if not have to do workarounds.
        // grabDismissAreaEvents seemed promising, but at least with onPressAndHold
        // makes background view scroll when moving mouse as if mouse button was still down.
        // grabDismissAreaEvents: false
        // Without this the popover jumps up at the start of the application. SDK bug?
        // Bug report has been made of these https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1152270
        visible: false
    }

    Dialog {
        id: notifyDialog
        visible: false
        Button {
            text: i18n.tr("Ok")
            onClicked: notifyDialog.hide()
        }
    }

    delegate: FolderListDelegate {
        id: delegate
        onClicked: {
            if (model.isDir) {
                if (model.isReadable && model.isExecutable) {
                    console.log("Changing to dir", model.filePath)
                    folderListModel.path = model.filePath
                } else {
                    notifyDialog.caller = delegate
                    notifyDialog.title = i18n.tr("Folder not accessible")
                    notifyDialog.text = i18n.tr("Can not access ") + model.fileName
                    notifyDialog.show()
                }
            } else {
                console.log("Non dir clicked")
                PopupUtils.open(Qt.resolvedUrl("FileActionDialog.qml"), delegate,
                                {
                                    fileName: model.fileName,
                                    filePath: model.filePath

                                 })
            }
        }

        onPressAndHold: {
            console.log("FolderListDelegate onPressAndHold")
            actionSelectionPopover.caller = delegate
            actionSelectionPopover.model = model
            actionSelectionPopover.show();
        }
    }

    Scrollbar {
        flickableItem: root
        align: Qt.AlignTrailing
    }
}
