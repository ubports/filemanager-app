import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../actions" as FMActions
import "../components" as Components
import "template" as Template

Template.Panel {
    id: bottomBar

    property var folderModel
    property var fileOperationDialog

    ActionList {
        id: defaultActions

        FMActions.Properties {
            onTriggered: {
                print(text)
                PopupUtils.open(Qt.resolvedUrl("../ui/FileDetailsPopover.qml"), mainView,{ "model": folderModel.model })
            }
        }

        FMActions.NewItem {
            visible: folderModel.model.isWritable
            onTriggered: {
                print(text)
                PopupUtils.open(Qt.resolvedUrl("../dialogs/CreateItemDialog.qml"), mainView, { folderModel: folderModel.model })
            }
        }
    }

    ActionBar {
        anchors.right: parent.right
        delegate: Components.TextualButtonStyle { }
        actions: defaultActions.children  // WORKAROUND: 'actions' is a non-NOTIFYable property
    }
}
