import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "templates"

ConfirmDialogWithInput {
    id: rootItem

    property var folderModel

    title: i18n.tr("Create file")
    text: i18n.tr("Enter name for new file")

    onAccepted: {
        console.log("Create file accepted", inputText)
        if (inputText !== '') {
            //FIXME: Actually create a new file!
        } else {
            console.log("Empty file name, ignored")
        }
    }
}
