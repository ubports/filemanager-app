/*
 * Copyright (C) 2013 Canonical Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Michael Spencer <sonrisesoftware@gmail.com>
 */
import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0
import Ubuntu.Components.Popups 1.0

/*
 * The Settings page holds global settings/preferences.
 *
 * TODO: Make sure this fits with the UI guidelines if
 * they are updated to include About/Settings info.
 */
Page {
    id: sheet
    title: i18n.tr("Settings")

    Component.onCompleted: {
        sheet.__leftButton.text = i18n.tr("Close")
        sheet.__foreground.style = Theme.createStyleComponent(Qt.resolvedUrl("../components/SuruSheetStyle.qml"), sheet)
    }

    Column {
        anchors.fill: parent
        anchors.margins: units.gu(-1)

        Standard {
            text: i18n.tr("Show Advanced Features")
            control: CheckBox {
                id: showAdvancedFeaturesCheckBox
                checked: showAdvancedFeatures
                onCheckedChanged: {
                    saveSetting("showAdvancedFeatures", showAdvancedFeaturesCheckBox.checked ? "true" : "false");
                }
            }
        }
    }
}
