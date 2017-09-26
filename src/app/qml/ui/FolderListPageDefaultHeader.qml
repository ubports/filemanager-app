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

    title: FmUtils.basename(folderModel.path)

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

    trailingActionBar.numberOfSlots: 2
    trailingActionBar.actions: [
        FMActions.PlacesBookmarks {
            visible: !showPanelAction.visible
            onTriggered: {
                var pp = pageStack.push(Qt.resolvedUrl("PlacesPage.qml"), { folderModel: rootItem.folderModel })
                pp.pathClicked.connect(function() {
                    pp.pageStack.pop()
                })
            }
        },

        FMActions.Settings {
            onTriggered: PopupUtils.open(Qt.resolvedUrl("ViewPopover.qml"), mainView, { folderListModel: folderModel.model })
        }
    ]

    // *** STYLE HINTS ***

    StyleHints { dividerColor: "transparent" }
}

