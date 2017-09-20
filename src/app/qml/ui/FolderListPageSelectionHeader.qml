import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../components" as Components
import "../actions" as FMActions

PageHeader {
    id: rootItem

    property var folderModel
    property var selectionManager: folderModel.model.selectionObject
    property bool selectionMode

    title: folderModel.basename(folderModel.path)

    contents: ListItemLayout {
        anchors.verticalCenter: parent.verticalCenter
        subtitle.text: rootItem.title
        title.text: i18n.tr("%1 item selected", "%1 items selected", folderModel.model.selectionObject.counter).arg(folderModel.model.selectionObject.counter)
    }

    extension: Components.PathHistoryRow {
        folderModel: rootItem.folderModel
    }

    leadingActionBar.actions: Action {
        text: i18n.tr("Cancel")
        iconName: "close"
        visible: selectionMode
        onTriggered: {
            console.log("FileSelector cancelled")
            if (isContentHub) {    // 'isContentHub' property declared in root QML file
                cancelFileSelector()
            } else {
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
            }
        }
    }


    // *** STYLE HINTS ***

    StyleHints { dividerColor: "transparent" }
}

