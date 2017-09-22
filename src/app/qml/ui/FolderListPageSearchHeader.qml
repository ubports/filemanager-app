import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../components" as Components
import "../actions" as FMActions

PageHeader {
    id: rootItem

    property var folderModel

    function searchF () {
        console.log("FileSearch started")

        var text = "*" + searchField.text + "*"
        folderModel.model.setIsRecursive(true)
        folderModel.model.nameFilters = [text]
    }

    title: folderModel.basename(folderModel.path)

    contents: TextField {
        id: searchField
        anchors {
            verticalCenter: parent.verticalCenter
            left: parent.left
            margins: units.gu(1)
        }

        inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
        placeholderText: i18n.tr("Search...")
        onAccepted: searchF()
    }

    extension: Rectangle {
        anchors { left: parent.left; right: parent.right }
        implicitHeight: units.gu(4)
        border.width: 1
        border.color: UbuntuColors.lightGrey
        Item {
            anchors.fill: parent
            anchors.leftMargin: units.gu(1)
            Text {
                anchors.verticalCenter: parent.verticalCenter
                text: (i18n.tr("%1 item", "%1 items", folderModel.count).arg(folderModel.count)
                       + " " + i18n.tr("found in") + " " + folderModel.path)
            }
        }
    }

    trailingActionBar.numberOfSlots: 2
    trailingActionBar.actions: [
        Action {
            text: i18n.tr("Close")
            iconName: "close"
            onTriggered: {
                console.log("FileSearch closed")
                folderModel.model.setIsRecursive(false)
                folderModel.model.nameFilters = ["*"]
                search = false
            }
        },
        Action {
            text: i18n.tr("Search")
            iconName: "find"
            onTriggered: searchF()
        }
    ]


    // *** STYLE HINTS ***

    StyleHints { dividerColor: "transparent" }
}

