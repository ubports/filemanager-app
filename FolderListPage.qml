import QtQuick 2.0
import Ubuntu.Components 0.1

Page {
    anchors.margins: units.gu(2)

    tools: ToolbarActions {
        back: Action {
            text: "Up"
            onTriggered: {
                // FIXME: The path should be normalized. This is just a quick and dirty
                // solution - this results in paths like "/home/user/Documents/../Videos/../Music
                // when used for a while. The path will grow indefinitely.
                folderListView.path = folderListView.path + "/.."
                console.log("Up triggered")
            }
        }
        active: folderListView.path != "/"
        lock: true
    }

    FolderListView {
        id: folderListView
        anchors.fill: parent
        path: "/"
    }
}
