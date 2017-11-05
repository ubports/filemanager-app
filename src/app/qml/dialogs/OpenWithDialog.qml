import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog {
    id: dialog
    modal: true
    title: i18n.tr("Open file")
    text: i18n.tr("What do you want to do with the clicked file?")
    property var model

    Button {
        id: previewButton
        text: i18n.tr("Preview")
        color: UbuntuColors.green
        onClicked: {
            PopupUtils.close(dialog)
            var props = {
                model: model
            }
            if(model.mimeType !== -1)
            pageStack.push(Qt.resolvedUrl("../ui/ImagePreview.qml"), props)
            else {
                Qt.openUrlExternally("video://" + filePath)
            }
        }
    }

    Button {
        id: openExternallyButton
        text: i18n.tr("Open with another app")
        color: UbuntuColors.green
        onClicked: {
            PopupUtils.close(dialog)
            openLocalFile(filePath)
        }
    }

    Button {
        id: propertiesButton
        text: i18n.tr("Properties")
        color: UbuntuColors.blue
        onClicked: {
            PopupUtils.open(Qt.resolvedUrl("../ui/FileDetailsPopover.qml"), mainView,{ "model": model })
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
