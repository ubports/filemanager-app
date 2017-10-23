import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

import "../components" as Components

Dialog {
    id: rootItem

    property var folderListModel

    __closeOnDismissAreaPress: true

    Component.onCompleted: {
        __foreground.itemSpacing = units.gu(0)
    }

    ListItem {
        ListItemLayout {
            anchors { left: parent.left; right: parent.right }
            anchors.leftMargin: units.gu(-2)
            anchors.rightMargin: units.gu(-2)

            subtitle.text: i18n.tr("Show Hidden Files")
            subtitle.textSize: Label.Medium

            Switch{
                SlotsLayout.position: SlotsLayout.Last

                checked: globalSettings.showHidden
                onCheckedChanged: globalSettings.showHidden = checked
            }
        }
    }

    Components.HorizontalOptionSelector {
        subtitle: i18n.tr("View As")
        selectedIndex: globalSettings.viewMethod
        model: [ i18n.tr("List"), i18n.tr("Icons") ]
        onSelectedIndexChanged: globalSettings.viewMethod = selectedIndex
    }

    Components.HorizontalOptionSelector {
        subtitle: i18n.tr("Grid size")
        visible: globalSettings.viewMethod === 1
        selectedIndex: globalSettings.gridSize
        model: [ i18n.tr("S"), i18n.tr("M"), i18n.tr("L"), i18n.tr("XL") ]
        onSelectedIndexChanged: globalSettings.gridSize = selectedIndex
    }

    Components.HorizontalOptionSelector {
        subtitle: i18n.tr("List size")
        visible: globalSettings.viewMethod === 0
        selectedIndex: globalSettings.listSize
        model: [ i18n.tr("S"), i18n.tr("M"), i18n.tr("L"), i18n.tr("XL") ]
        onSelectedIndexChanged: globalSettings.listSize = selectedIndex
    }

    Components.HorizontalOptionSelector {
        subtitle: i18n.tr("Sort By")
        selectedIndex: globalSettings.sortBy
        model: [ i18n.tr("Name"), i18n.tr("Date") ]
        onSelectedIndexChanged: globalSettings.sortBy = selectedIndex
    }

    Components.HorizontalOptionSelector {
        subtitle: i18n.tr("Sort Order")
        selectedIndex: globalSettings.sortOrder
        model: [ "A ➡ Z", "Z ➡ A" ]
        onSelectedIndexChanged: globalSettings.sortOrder = selectedIndex
    }

    Components.HorizontalOptionSelector {
        subtitle: i18n.tr("Theme")
        selectedIndex: globalSettings.darkTheme
        model: [ i18n.tr("Light"), i18n.tr("Dark") ]
        onSelectedIndexChanged: globalSettings.darkTheme = selectedIndex
    }
}
