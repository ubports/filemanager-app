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
 * Authored by: Arto Jalkanen <ajalkane@gmail.com>
 */
import QtQuick 2.0
import Ubuntu.Components 0.1
import org.nemomobile.folderlistmodel 1.0
import Ubuntu.Components.Popups 0.1

/*!
    \brief MainView with Tabs element.
           First Tab has a single Label and
           second Tab has a single ToolbarAction.
*/

MainView {
    id: root
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "filemanager"
    applicationName: "ubuntu-filemanager-app"

    width: units.gu(100)
    height: units.gu(75)

    property alias filemanager: root

    property bool wideAspect: width >= units.gu(80)

    // Default settings
    property var settings: {"showAdvancedFeatures": false}

    property bool needsRefreshSettings: true

    // Individual settings, used for bindings
    property bool showAdvancedFeatures: false

    headerColor: "#303030"
    backgroundColor: "#505050"
    footerColor: "#707070"

    FolderListPage {
        id: folderPage
        objectName: "folderPage"

        folder: homeFolder
    }

    Component {
        id: settingsSheet

        SettingsSheet {
            objectName: "settingsSheet"
        }
    }

    Storage {
        id: storage
    }

    function showSettings() {
        PopupUtils.open(settingsSheet)
    }

    function reloadSettings() {
        showAdvancedFeatures = settings["showAdvancedFeatures"] === "true" ? true : false
        print("showAdvancedFeatures <=", showAdvancedFeatures)
    }

    function refreshSettings() {
        if (needsRefreshSettings) {
            storage.getSettings(function(storedSettings) {
                for(var settingName in storedSettings) {
                    print(settingName, "=", storedSettings[settingName])
                    settings[settingName] = storedSettings[settingName]
                }

                reloadSettings()
            })

            needsRefreshSettings = false
        }
    }

    function saveSetting(name, value) {
        // Check if the setting was changed
        if(settings[name] !== value) {
            print(name, "=>", value)
            storage.saveSetting(name, value)
            needsRefreshSettings = true
        }
    }

    Component.onCompleted: refreshSettings();
}
