import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "templates"

ConfirmDialog {
    id: rootItem

    property var folderModel
    property var fileOperationDialog
    property string filePath
    property string fileName

    title: i18n.tr("Delete")
    text: i18n.tr("Are you sure you want to permanently delete '%1'?").arg(fileName)

    onAccepted: {
        console.log("Delete accepted for filePath, fileName", filePath, fileName)

        fileOperationDialog.startOperation(i18n.tr("Deleting files"))
        console.log("Doing delete")
        folderModel.rm(filePath)
    }
}
