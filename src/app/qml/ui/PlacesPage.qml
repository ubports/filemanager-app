import QtQuick 2.4
import Ubuntu.Components 1.3

import "../backend"
import "../components" as Components
import "../actions" as FMActions

Page {
    id: root

    property FolderListModel folderModel

    signal pathClicked()

    header: PageHeader {
        title: i18n.tr("Places")

        /*trailingActionBar {
            anchors.rightMargin: 0
            delegate: Components.TextualButtonStyle {}
            actions: Action {
                text: i18n.tr("Edit")
                onTriggered: rootItem.showEditSources()
            }
        }*/

        StyleHints { dividerColor: "transparent" }
    }

    ScrollView {
        id: scrollView
        anchors.fill: parent
        anchors.topMargin: root.header.height

        ListView {
            anchors.fill: parent
            model: folderModel.places

            delegate: ListItem {
                divider.visible: false
                height: units.gu(6)

                Rectangle {
                    anchors.fill: parent
                    color: Qt.rgba(0, 0, 0, 0.2)
                    visible: model.path == folderModel.path
                }

                ListItemLayout {
                    anchors.fill: parent
                    title.text: FmUtils.pathTitle(model.path)

                    Icon {
                        SlotsLayout.position: SlotsLayout.Leading
                        width: units.gu(4); height: width
                        name: folderModel.model.getIcon(model.path)
                    }
                }

                leadingActions: ListItemActions {
                    actions: FMActions.Delete {
                        visible: enabled
                        enabled: !folderModel.places.isDefaultLocation(model.path)
                            && !folderModel.places.isUserMountDirectory(model.path)
                        onTriggered: folderModel.places.removeItem(model.index)
                    }
                }

                onClicked: {
                    folderModel.goTo(model.path)
                    root.pathClicked()
                }
            }
        }
    }
}
