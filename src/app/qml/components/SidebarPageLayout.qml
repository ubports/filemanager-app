import QtQuick 2.4
import Ubuntu.Components 1.3

import "../actions" as Actions

PageTreeNode {
    id: rootItem
    anchors.fill: parent

    property alias sidebarLoader: sidePageLoader
    property alias mainLoader: mainLoader

    property bool  sidebarActive: true
    property alias sidebarWidth: sidebar.width
    property alias sidebarMinimumWidth: sidebar.minimumWidth
    property alias sidebarMaximumWidth: sidebar.maximumWidth

    property alias sidebarResizing: sidebar.resizing

    property Action showPanelAction: Actions.ShowPanel {
        checkable: true
        visible: enabled
        enabled: rootItem.sidebarActive
    }

    ResizeableSidebar {
        id: sidebar
        visible: showPanelAction.checked && sidebarActive
        anchors {
            left: parent.left
            leftMargin: sidebar.visible ? 0 : -sidebar.width
        }

        Behavior on anchors.leftMargin {
            UbuntuNumberAnimation {}
        }

        Loader {
            id: sidePageLoader
            anchors.fill: parent
            onLoaded: console.log("[SidebarPageLayout] Sidebar loaded")
        }
    }

    Loader {
        id: mainLoader
        anchors {
            left: sidebar.right
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
    }
}
