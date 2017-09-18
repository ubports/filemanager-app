import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "templates"

ConfirmDialog {
    id: rootItem

    property string filePath
    property string fileName
    property string archiveType
    property var folderListPage

    title: i18n.tr("Extract Archive")
    text: i18n.tr("Are you sure you want to extract '%1' here?").arg(fileName)

    onAccepted: {
        folderListPage.extractArchive(filePath, fileName, archiveType)
    }
}
