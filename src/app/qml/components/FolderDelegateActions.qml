import QtQuick 2.4
import QtQml 2.2
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../actions" as FMActions

QtObject {
    id: rootItem

    property var folderListPage
    property var folderModel
    property var fileOperationDialog

    function itemLongPress(delegate, model) {
        console.log("FolderListDelegate onPressAndHold")
        var props = { model: model }
        PopupUtils.open(__actionSelectionPopoverComponent, delegate, props)
    }

    function listLongPress() {
        isContentHub = false
        fileSelectorMode = true
        fileSelector.fileSelectorComponent = pageStack
    }

    function itemClicked(model) {
        if (model.isBrowsable) {
            console.log("browsable path="+model.filePath+" isRemote="+model.isRemote+" needsAuthentication="+model.needsAuthentication)
            if ((model.isReadable && model.isExecutable) ||
                    (model.isRemote && model.needsAuthentication) //in this case it is necessary to generate the signal needsAuthentication()
                    ) {
                console.log("Changing to dir", model.filePath)
                folderModel.goTo(model.filePath)
            } else {
                var props = {
                    title: i18n.tr("Folder not accessible"),
                    // TRANSLATORS: this refers to a folder name
                    text: i18n.tr("Can not access %1").arg(model.fileName)
                }

                PopupUtils.open(Qt.resolvedUrl("../dialogs/NotifyDialog.qml"), delegate, props)
            }
        } else {
            console.log("Non dir clicked")
            if (fileSelectorMode) {
                folderModel.model.selectionObject().select(model.index,false,true)
            } else if (!folderSelectorMode){
                openFile(model)
            }
        }
    }

    function itemDateAndSize(model) {
        var strDate = Qt.formatDateTime(model.modifiedDate, Qt.DefaultLocaleShortDate);
        //local file systems always have date and size for both files and directories
        //remote file systems may have not size for directories, it comes as "Unknown"
        if (strDate) {
            strDate += ", " + model.fileSize //show the size even it is "Unknown"
        }
        return strDate;
    }

    property ActionList leadingActions: ActionList {
        FMActions.Delete {
            visible: folderModel.model.isWritable
            onTriggered: {
                var props = {
                    "folderModel": folderModel.model,
                    "fileOperationDialog": fileOperationDialog,
                    "filePath" : model.filePath,
                    "fileName" : model.fileName
                }
                PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmSingleDeleteDialog.qml"), mainView, props)
            }
        }

        FMActions.Rename {
            visible: folderModel.model.isWritable
            onTriggered: {
                var props = {
                    "modelRow" : model.index,
                    "inputText" : model.fileName,
                    "folderModel": folderModel.model
                }
                PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmRenameDialog.qml"), mainView, props)
            }
        }
    }

    property ActionList trailingActions: ActionList {
        FMActions.ArchiveExtract {
            visible: folderModel.getArchiveType(model.fileName) !== ""
            onTriggered: folderListPage.openFile(model, true)
        }

        FMActions.Properties {
            onTriggered: {
                var props = {
                    "model": model
                }
                PopupUtils.open(Qt.resolvedUrl("../ui/FileDetailsPopover.qml"), mainView, props)
            }
        }

        FMActions.FileCopy {
            onTriggered: {
                folderModel.model.copyIndex(model.index)
            }
        }

        FMActions.FileCut {
            visible: folderModel.model.isWritable
            onTriggered: {
                folderModel.model.cutIndex(model.index)
            }
        }

        FMActions.Share {
            visible: !model.isDir
            onTriggered: folderListPage.openFile(model, true)
        }
    }


    // *** COMPONENTS ***

    property Component __actionSelectionPopoverComponent: Component {
        ActionSelectionPopover {
            grabDismissAreaEvents: true
            property var model

            actions: ActionList {
                Component.onCompleted: {
                    // Build a single list of actions from the two lists above
                    var tmp = trailingActions.actions
                    var copy = []
                    var i;

                    for (i = 0; i < tmp.length; ++i) {
                        copy[i] = tmp[i]
                    }

                    tmp = leadingActions.actions
                    var j = copy.length
                    for (i = 0; i < tmp.length; ++i) {
                        copy[i+j] = tmp[i]
                    }

                    actions = copy;
                }
            }
        }
    }
}