import QtQuick 2.4
import Ubuntu.Components 1.3

Item {
    id: rootItem
    anchors { left: parent.left; right: parent.right }
    height: visible ? units.gu(6) : 0
    enabled: visible
}
