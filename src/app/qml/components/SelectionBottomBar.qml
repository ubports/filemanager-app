import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../backend"

// TODO: check origin of properties used in bindings

Rectangle {
    id: bottomBar
    height: visible ? bottomBarButtons.height : 0
    color: theme.palette.normal.background
    enabled: visible

    property FolderListModel folderModel
    property var selectionManager: folderModel.model.selectionObject
    property var fileOperationDialog

    property bool __actionsEnabled: (selectionManager.counter > 0) || (folderSelectorMode && folderModel.model.isWritable)
    property bool __actionsVisible: selectionMode

    Flow {
        id: bottomBarButtons
        height: units.gu(7)
        spacing: units.gu(2)
        anchors.horizontalCenter: parent.horizontalCenter

        Button {
            property bool showText: false
            text: i18n.tr("Cut")
            iconName: "edit-cut"
            width: units.gu(5)
            height: units.gu(5)
            anchors.topMargin: units.gu(1)
            color: "white"
            enabled: __actionsEnabled
            visible: __actionsVisible && folderModel.model.isWritable && !isContentHub    // 'isContentHub' property declared in root QML file
            onClicked: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                pageModel.model.cutPaths(selectedAbsPaths)
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
                folderModel.helpClipboard = true
            }
        }

        Button {
            property bool showText: false
            text: i18n.tr("Copy")
            iconName: "edit-copy"
            width: units.gu(5)
            height: units.gu(5)
            anchors.topMargin: units.gu(1)
            color: "white"
            enabled: __actionsEnabled
            visible: __actionsVisible && !isContentHub    // 'isContentHub' property declared in root QML file
            onClicked: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                pageModel.model.copyPaths(selectedAbsPaths)
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
                folderModel.helpClipboard = true
            }
        }

        Button {
            property bool showText: false
            text: i18n.tr("Delete")
            iconName: "edit-delete"
            width: units.gu(5)
            height: units.gu(5)
            anchors.topMargin: units.gu(1)
            color: "white"
            enabled: __actionsEnabled
            visible: __actionsVisible && folderModel.model.isWritable && !isContentHub    // 'isContentHub' property declared in root QML file
            onClicked: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();

                var props = {
                    "paths" : selectedAbsPaths,
                    "folderModel": pageModel.model,
                    "fileOperationDialog": fileOperationDialog
                }

                PopupUtils.open(Qt.resolvedUrl("../dialogs/ConfirmMultipleDeleteDialog.qml"), mainView, props)
//                selectionManager.clear()
//                fileSelectorMode = false
//                fileSelector.fileSelectorComponent = null
            }
        }

        Button {
            property bool showText: false
            text: i18n.tr("Select")
            iconName: "tick"
            width: units.gu(5)
            height: units.gu(5)
            anchors.topMargin: units.gu(1)
            color: "white"
            enabled: __actionsEnabled
            visible: __actionsVisible && isContentHub     // 'isContentHub' property declared in root QML file
            onClicked: {
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
    }
}
