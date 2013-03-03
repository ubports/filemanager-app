import QtQuick 2.0
import Ubuntu.Components 0.1
import org.nemomobile.folderlistmodel 1.0

ListView {
    id: root

    model: FolderListModel {
        id: folderListModel
        path: "/"
    }


    delegate: Item {
        width: parent.width
        height: childrenRect.height

        Label {
            id: fileName
            anchors.left: parent.left
            anchors.right: fileSize.left
            anchors.margins: units.gu(2)
            wrapMode: Text.NoWrap

            text: model.fileName
        }
        Label {
            id: fileSize
            anchors.right: modifiedDate.left
            anchors.margins: units.gu(2)

            text: model.fileSize
        }
        Label {
            id: modifiedDate
            anchors.right: parent.right
            anchors.margins: units.gu(2)

            text: model.modifiedDate
        }
    }

}
