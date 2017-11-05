import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

// TODO: Path validation in inputText: disable buttons if new object name is not valid
// TODO: Handle failure of mkpath/touch

Dialog {
    id: dialog

    property var folderModel

    title: i18n.tr("Create Item")
    text: i18n.tr("Enter name for new item")

    TextField {
        id: inputText
        placeholderText: i18n.tr("Item name")
        focus: true
    }

    Button {
        id: createFile
        text: i18n.tr("Create file")
        color: UbuntuColors.green
        onClicked: {
            inputText.focus = false
            console.log("Create file accepted", inputText.text)

            if (inputText.text !== '') {
                var fileName = inputText.text.trim()
                folderModel.touch(fileName)
            } else {
                console.log("Empty file name, ignored")
            }

            PopupUtils.close(dialog)
        }
    }

    Button {
        id: createFolder
        text: i18n.tr("Create Folder")
        color: UbuntuColors.green
        onClicked: {
            inputText.focus = false
            console.log("Create folder accepted", inputText.text)
            if (inputText.text !== '') {
                var folderName = inputText.text.trim()
                folderModel.mkdir(folderName)
            } else {
                console.log("Empty directory name, ignored")
            }
            PopupUtils.close(dialog)
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
