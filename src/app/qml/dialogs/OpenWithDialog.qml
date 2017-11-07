import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog {
    id: dialog
    modal: true
    title: i18n.tr("Open file")
    text: i18n.tr("What do you want to do with the clicked file?")
    property var model

    property bool previewButtonVisible: false
    property bool extractButtonVisible: false

    signal showPreview()
    signal extractArchive()
    signal openWith()
    signal showProperties()

    Button {
        id: previewButton
        text: i18n.tr("Preview")
        color: UbuntuColors.green
        visible: previewButtonVisible
        onClicked: {
            PopupUtils.close(dialog)
            showPreview()
        }
    }

    Button {
        id: extractButton
        text: i18n.tr("Extract archive")
        color: UbuntuColors.green
        visible: extractButtonVisible
        onClicked: {
            PopupUtils.close(dialog)
            extractArchive()
        }
    }

    Button {
        id: openExternallyButton
        text: i18n.tr("Open with another app")
        color: UbuntuColors.green
        onClicked: {
            PopupUtils.close(dialog)
            openWith()
        }
    }

    Button {
        id: propertiesButton
        text: i18n.tr("Properties")
        color: UbuntuColors.blue
        onClicked: {
            PopupUtils.close(dialog)
            showProperties()
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
