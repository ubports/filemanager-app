import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import org.nemomobile.folderlistmodel 1.0

Dialog {
    id: root
    property FolderListModel folderListModel
    property string descriptionPrepend: i18n.tr("Operation in progress")

    title: "File operation"
    text: descriptionPrepend

    ProgressBar {
        id: progress
        minimumValue: 0.0
        maximumValue: 100.0
        value: 0.0
    }

    Button {
        text: i18n.tr("Cancel")
        onClicked: {
            console.log("Cancelling file progress action")
            folderListModel.cancelAction()
            PopupUtils.close(root)
        }
    }

    Connections {
        target: folderListModel
        onProgress: {
            console.log("On progress ", curItem, totalItems, percent)
            progress.value = percent
            // TODO: is this a bug in backend? The first received signal is curItem=0, totalItems=0. Work around:
            if (!(curItem == 0 && totalItems == 0)) {
                if (curItem == totalItems) {
                    console.log("All files processed, closing progress dialog")
                    PopupUtils.close(root)
                } else {
                    root.text = descriptionPrepend + " " + curItem + "/" + totalItems
                }
            }
        }
    }
}
