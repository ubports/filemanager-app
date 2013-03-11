import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1
import org.nemomobile.folderlistmodel 1.0

ListView {
    id: root

    property FolderListModel folderListModel
    property string path: folderListModel.path
    model: folderListModel

    ActionSelectionPopover {
        id: actionSelectionPopover
        property string filePath

        actions: ActionList {
            Action {
                text: i18n.tr("Add as bookmark")
                // TODO
                onTriggered: print(text)
            }
            Action {
                text: i18n.tr("Show details")
                onTriggered: {
                    print(text)
                    PopupUtils.open(Qt.resolvedUrl("FileDetailsPopover.qml"),
                                    actionSelectionPopover.caller,
                                    { "filePath": actionSelectionPopover.filePath } )
                }
            }
        }
        // TODO: problem: clicking outside popup makes the click go through to the
        // folder listview, so for example you'd change directory while only trying
        // to dismiss the popup. Maybe SDK bug, if not have to do workarounds.
        // grabDismissAreaEvents seemed promising, but at least with onPressAndHold
        // makes background view scroll when moving mouse as if mouse button was still down.
        // grabDismissAreaEvents: false
        // Without this the popover jumps up at the start of the application. SDK bug?
        // Bug report has been made of these https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1152270
        visible: false
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
        onPressAndHold: {
            console.log("FolderListDelegate onPressAndHold")
            actionSelectionPopover.caller = delegate
            actionSelectionPopover.filePath = filePath
            actionSelectionPopover.show();
        }
    }


}
