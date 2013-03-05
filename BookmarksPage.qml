import QtQuick 2.0
import Ubuntu.Components 0.1

Page {
    anchors.margins: units.gu(2)

    Column {
        anchors.centerIn: parent
        Label {
            id: label
            objectName: "label"

            text: i18n.tr("TODO: listing of bookmarks")
        }
    }
}
