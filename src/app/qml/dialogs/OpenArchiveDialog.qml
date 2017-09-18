import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog {
    id: dialog
    modal: true
    title: i18n.tr("Archive file")
    text: i18n.tr("Do you want to extract the archive here?")
    property string filePath
    property string fileName
    property string archiveType
    property var folderListPage

    Button {
        id: extractButton
        text: i18n.tr("Extract archive")
        color: UbuntuColors.green
        onClicked: {
            PopupUtils.close(dialog)
            folderListPage.extractArchive(filePath, fileName, archiveType)
        }
    }

    Button {
        id: openExternallyButton
        text: i18n.tr("Open with another app")
        color: UbuntuColors.red
        onClicked: {
            PopupUtils.close(dialog)
            folderListPage.openLocalFile(filePath)
        }
    }

    Button {
        id: cancelButton
        text: i18n.tr("Cancel")
        color: UbuntuColors.graphite
        onClicked: {
            PopupUtils.close(dialog)
        }
    }
}
