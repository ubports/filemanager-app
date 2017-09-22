import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../components" as Components
import "../actions" as FMActions

PageHeader {
    id: rootItem

    // temp
    property var fileOperationDialog
    property var folderModel
    property var showPanelAction

    title: folderModel.basename(folderModel.path)

    contents: ListItemLayout {
        anchors.verticalCenter: parent.verticalCenter
        title.text: rootItem.title
        subtitle.text: i18n.tr("%1 item", "%1 items", folderModel.count).arg(folderModel.count)
    }

    extension: Components.PathHistoryRow {
        folderModel: rootItem.folderModel
    }

    FMActions.GoBack {
        id: goBackAction
        onTriggered: folderModel.goBack()
    }

    leadingActionBar.actions: showPanelAction.visible ? showPanelAction : goBackAction

    trailingActionBar.numberOfSlots: 4
    trailingActionBar.actions: [
        FMActions.Settings {
            onTriggered: PopupUtils.open(Qt.resolvedUrl("ViewPopover.qml"), mainView, { folderListModel: folderModel.model })
        },

        FMActions.Search {
            onTriggered: {
                print(text)
                search = true
            }
        },

        FMActions.NewFolder {
            visible: folderModel.model.isWritable
            onTriggered: {
                print(text)
                PopupUtils.open(Qt.resolvedUrl("../dialogs/CreateFolderDialog.qml"), mainView, { folderModel: folderModel.model })
            }
        },

        FMActions.Properties {
            onTriggered: {
                print(text)
                PopupUtils.open(Qt.resolvedUrl("FileDetailsPopover.qml"), mainView,{ "model": folderModel.model })
            }
        }
    ]

    // *** STYLE HINTS ***

    StyleHints { dividerColor: "transparent" }
}

