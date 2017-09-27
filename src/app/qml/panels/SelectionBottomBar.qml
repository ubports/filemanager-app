import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../backend"
import "template" as Template
import "../components" as Components

// TODO: check origin of properties used in bindings

Template.Panel {
    id: bottomBar

    property FolderListModel folderModel
    property var selectionManager: folderModel.model.selectionObject
    property var fileOperationDialog

    property bool __actionsEnabled: (selectionManager.counter > 0) || (folderSelectorMode && folderModel.model.isWritable)
    property bool __actionsVisible: selectionMode

    ActionList {
        id: selectionActions

        Action {
            property bool smallText: true
            text: i18n.tr("Delete")
            iconName: "edit-delete"
            enabled: __actionsEnabled
            visible: __actionsVisible && folderModel.model.isWritable
            onTriggered: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();

                var props = {
                    "paths" : selectedAbsPaths,
                    "folderModel": folderModel.model,
                    "fileOperationDialog": fileOperationDialog
                }

                PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmMultipleDeleteDialog.qml"), mainView, props)
            }
        }

        Action {
            property bool smallText: true
            text: i18n.tr("Copy")
            iconName: "edit-copy"
            enabled: __actionsEnabled
            visible: __actionsVisible
            onTriggered: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                folderModel.model.copyPaths(selectedAbsPaths)
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
            }
        }

        Action {
            property bool smallText: true
            text: i18n.tr("Cut")
            iconName: "edit-cut"
            enabled: __actionsEnabled
            visible: __actionsVisible && folderModel.model.isWritable
            onTriggered: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                folderModel.model.cutPaths(selectedAbsPaths)
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
            }
        }
    }

    ActionBar {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        delegate: Components.TextualButtonStyle { }
        numberOfSlots: 3
        actions: selectionActions.children  // WORKAROUND: 'actions' is a non-NOTIFYable property
    }
}
