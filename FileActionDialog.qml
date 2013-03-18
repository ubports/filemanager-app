import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

Dialog {
    id: root

    property string fileName
    property string filePath

    title: i18n.tr("Choose action")
    text: i18n.tr("For file: ") + fileName

    Button {
        text: i18n.tr("Open")
        onClicked: {
            console.log("Opening file", filePath)
            Qt.openUrlExternally("file://" + filePath)
            onClicked: PopupUtils.close(root)
        }
    }

    Button {
        text: i18n.tr("Cancel")
        onClicked: PopupUtils.close(root)
    }
}
