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
import Ubuntu.Unity.Action 1.0 as UnityActions
import U1db 1.0 as U1db

/*!
    \brief MainView with Tabs element.
           First Tab has a single Label and
           second Tab has a single ToolbarAction.
*/

MainView {
    id: root
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "filemanager"
    applicationName: "com.ubuntu.filemanager"

    width: units.gu(100)
    height: units.gu(75)

    property alias filemanager: root

    property bool wideAspect: width >= units.gu(80)

    headerColor: "#464646"
    backgroundColor: "#797979"
    footerColor: "#808080"

    // HUD Actions
    Action {
        id: settingsAction
        text: i18n.tr("Settings")
        description: i18n.tr("Change app settings")
        onTriggered: pageStack.push(settingsPage)
    }
    actions: [settingsAction]

    property var pageStack: pageStack

    PageStack {
        id: pageStack

        Tabs {
            id: tabs

            Tab {
                title: page.title
                page: FolderListPage {
                    id: folderPage
                    objectName: "folderPage"

                    folder: homeFolder
                }
            }
        }

        Component.onCompleted: {
            pageStack.push(tabs)
            pageStack.push(settingsPage)
            pageStack.pop()
        }
    }

    property var showToolbar: wideAspect ? true : undefined

    states: [
        State {
            when: showToolbar && toolbar.tools.opened && toolbar.tools.locked

            PropertyChanges {
                target: pageStack
                anchors.bottomMargin: -root.toolbar.triggerSize
            }
        }
    ]

    /* Settings Storage */

    U1db.Database {
        id: storage
        path: "ubuntu-filemanager-app.db"
    }

    U1db.Document {
        id: settings

        database: storage
        docId: 'settings'
        create: true

        defaults: {
            showAdvancedFeatures: false
        }
    }

    SettingsPage {
        id: settingsPage
    }

    // Individual settings, used for bindings
    property bool showAdvancedFeatures: false

    function getSetting(name, def) {
        var tempContents = {};
        tempContents = settings.contents
        var value = tempContents.hasOwnProperty(name)
                ? tempContents[name]
                : settings.defaults.hasOwnProperty(name)
                  ? settings.defaults[name]
                  : def
        //print(name, JSON.stringify(def), JSON.stringify(value))
        return value
    }

    function saveSetting(name, value) {
        if (getSetting(name) !== value) {
            //print(name, "=>", value)
            var tempContents = {}
            tempContents = settings.contents
            tempContents[name] = value
            settings.contents = tempContents

            reloadSettings()
        }
    }

    function reloadSettings() {
        showAdvancedFeatures = getSetting("showAdvancedFeatures", false)
    }

    Component.onCompleted: reloadSettings()

    function getIcon(name) {
        return Qt.resolvedUrl("icons/" + name + ".png")
    }
}
