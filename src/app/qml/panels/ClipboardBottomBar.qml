import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../actions" as FMActions
import "../components" as Components

Rectangle {
    id: bottomBar
    height: visible ? units.gu(6) : 0
    color: theme.palette.normal.background
    enabled: visible

    property var folderModel
    property var fileOperationDialog

    ActionList {
        id: clipboardActions

        FMActions.FilePaste {
            property bool showText: false
            clipboardUrlsCounter: folderModel.model.clipboardUrlsCounter
            visible: folderModel.model.clipboardUrlsCounter > 0
            onTriggered: {
                console.log("Pasting to current folder items of count " + folderModel.model.clipboardUrlsCounter)
                fileOperationDialog.startOperation(i18n.tr("Paste files"))
                folderModel.model.paste()
            }
        }

        FMActions.FileClearSelection {
            property bool showText: false
            clipboardUrlsCounter: folderModel.model.clipboardUrlsCounter
            visible: folderModel.model.clipboardUrlsCounter > 0
            onTriggered: {
                console.log("Clearing clipboard")
                // FIXME: Seems not to clear actually
                folderModel.model.clearClipboard()
            }
        }
    }

    ListItemLayout {
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        title.text: i18n.tr("Clipboard")
        subtitle.text: i18n.tr("%1 item", "%1 items", folderModel.model.clipboardUrlsCounter).arg(folderModel.model.clipboardUrlsCounter)
    }

    ActionBar {
        anchors.right: parent.right
        delegate: Components.TextualButtonStyle { }
        actions: clipboardActions.actions
    }
}
