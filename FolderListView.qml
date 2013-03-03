import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import org.nemomobile.folderlistmodel 1.0

ListView {
    id: root
    property alias path: folderListModel.path

    model: FolderListModel {
        id: folderListModel
    }

    delegate: FolderListDelegate {
        id: delegate
        onClicked: {
            if (model.isDir) {
                console.log("Changing to dir", model.filePath)
                folderListModel.path = model.filePath
            } else {
                console.log("Non dir clicked")
            }
        }
    }
}
