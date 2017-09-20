import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "templates"

ConfirmDialogWithInput {
    id: rootItem

    property var folderModel

    title: i18n.tr("Create folder")
    text: i18n.tr("Enter name for new folder")

    onAccepted: {
        console.log("Create folder accepted", inputText)
        if (inputText !== '') {
            var folderName = inputText.trim()
            if (folderModel.mkdir(folderName)) {
                folder = folderModel.path + "/" + folderName
            }
        } else {
            console.log("Empty directory name, ignored")
        }
    }
}
