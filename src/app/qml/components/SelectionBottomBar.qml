import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: bottomBar
    height: visible ? units.gu(6) : 0

    property var folderModel
    property var selectionManager: folderModel.model.selectionObject()

    property bool __actionsEnabled: (selectionManager.counter > 0) || (folderSelectorMode && folderModel.model.isWritable)
    property bool __actionsVisible: selectionMode && !isContentHub

    ActionList {
        id: selectionActions

        Action {
            property bool showText: false
            text: i18n.tr("Select")
            iconName: "tick"
            enabled: __actionsEnabled
            visible: __actionsVisible
            onTriggered: {
                var selectedAbsUrls = []
                if (folderSelectorMode) {
                    selectedAbsUrls = [ folder ]
                } else {
                    var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                    // For now support only selection in filesystem
                    selectedAbsUrls = selectedAbsPaths.map(function(item) {
                        return "file://" + item;
                    });
                }
                console.log("FileSelector OK clicked, selected items: " + selectedAbsUrls)
                acceptFileSelector(selectedAbsUrls)
            }
        }

        Action {
            property bool showText: false
            text: i18n.tr("Cancel")
            iconName: "edit-clear"
            visible: selectionMode
            onTriggered: {
                console.log("FileSelector cancelled")
                if (isContentHub) {
                    cancelFileSelector()
                } else {
                    selectionManager.clear()
                    fileSelectorMode = false
                    fileSelector.fileSelectorComponent = null
                }
            }
        }

        Action {
            text: i18n.tr("Cut")
            iconName: "edit-cut"
            enabled: __actionsEnabled
            visible: __actionsVisible && folderModel.model.isWritable
            onTriggered: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                pageModel.model.cutPaths(selectedAbsPaths)
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
            }
        }

        Action {
            text: i18n.tr("Copy")
            iconName: "edit-copy"
            enabled: __actionsEnabled
            visible: __actionsVisible
            onTriggered: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                pageModel.model.copyPaths(selectedAbsPaths)
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
            }
        }

        Action {
            text: i18n.tr("Delete")
            iconName: "edit-delete"
            enabled: __actionsEnabled
            visible: __actionsVisible && folderModel.model.isWritable
            onTriggered: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();

                var props = {
                    "paths" : selectedAbsPaths,
                    "folderModel": pageModel.model,
                    "fileOperationDialog": fileOperationDialog
                }

                PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmMultipleDeleteDialog.qml"), mainView, props)
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
            }
        }
    }

    ActionBar {
        id: bottomBarButtons
        anchors.fill: parent
        numberOfSlots: MathUtils.clamp(bottomBar.width/units.gu(12), 3, 6)
        delegate: TextualButtonStyle { }
        actions: selectionActions.actions
    }
}
