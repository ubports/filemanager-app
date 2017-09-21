import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog {
    id: dialog
    modal: true
    title: i18n.tr("Open file")
    text: i18n.tr("What do you want to do with the clicked file?")
    property string fileType

    property var model

    Button {
        id: previewButton
        text: i18n.tr("Preview")
        color: UbuntuColors.green
        onClicked: {
            PopupUtils.close(dialog)
            var props = {
                model: model,
                path: model.filePath
            }
            pageStack.push(Qt.resolvedUrl("../ui/" + fileType + "Preview.qml"), props)
        }
    }

    Button {
        id: openExternallyButton
        text: i18n.tr("Open with another app")
        color: UbuntuColors.red
        onClicked: {
            PopupUtils.close(dialog)
            openLocalFile(filePath)
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
