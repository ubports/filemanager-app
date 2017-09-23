import QtQuick 2.4
import Ubuntu.Components 1.3

Rectangle {
    id: rootItem

    default property alias layoutContent: layout.children

    color: theme.palette.normal.background
    height: layout.height

    anchors {
        left: parent.left
        right: parent.right
        bottom: parent.bottom
    }

    Column {
        id: layout
        anchors {
            left: parent.left
            right: parent.right
        }
    }
}
