import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1
import Ubuntu.Components.Popups 0.1

/*
 * The Settings sheet holds global settings/preferences.
 *
 * TODO: Make sure this fits with the UI guidelines if
 * they are updated to include About/Settings info.
 */
ComposerSheet {
    id: root

    title: i18n.tr("Settings")

    Binding {
        target: root.__foreground
        property: "minHeight"
        value: Math.max(units.gu(75), root.contentsHeight)
    }

    Column {
        anchors.fill: parent

        Standard {
            text: i18n.tr("Show Advanced Features")
            control: CheckBox {
                id: showAdvancedFeaturesCheckBox
                checked: showAdvancedFeatures
            }
        }
    }

    onConfirmClicked: {
        saveSetting("showAdvancedFeatures", showAdvancedFeaturesCheckBox.checked ? "true" : "false");

        // ... Handling of other settings here ...

        refreshSettings()

        PopupUtils.close(root)
    }
}
