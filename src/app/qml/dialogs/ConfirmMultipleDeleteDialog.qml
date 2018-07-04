import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "templates"

ConfirmDialog {
    id: rootItem

    property var folderModel
    property var fileOperationDialog
    property var paths

    title: i18n.tr("Delete")
    text: i18n.tr("Are you sure you want to permanently delete '%1'?").arg(i18n.tr("these files"))

    onAccepted: {
        fileOperationDialog.startOperation(i18n.tr("Deleting files"))
        console.log("Doing delete")
        folderModel.removePaths(paths)
        selectionManager.clear()
        fileSelectorMode = false
        fileSelector.fileSelectorComponent = null
    }
}
