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
    id: mainView
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "filemanager"
    applicationName: "com.ubuntu.filemanager"

    width: units.gu(100)
    height: units.gu(75)

    property alias filemanager: mainView

    property bool wideAspect: width >= units.gu(80)

    property bool allowSidebarExpanded: width >= units.gu(80)

    onAllowSidebarExpandedChanged: {
        if (!allowSidebarExpanded)
            saveSetting("collapsedSidebar", true)
    }

    property bool showSidebar: width >= units.gu(50)

    property bool showToolbar: width >= units.gu(80)

    headerColor: "#464646"
    backgroundColor: "#797979"
    footerColor: "#808080"

    // HUD Actions
    Action {
        id: settingsAction
        text: i18n.tr("Settings")
        description: i18n.tr("Change app settings")
        iconSource: getIcon("settings")
        onTriggered: pageStack.push(settingsPage)
    }
    actions: [settingsAction]

    property var pageStack: pageStack

    property var folderTabs: ["~"]

    function openTab(folder) {
        var list = folderTabs
        list.push(folder)
        folderTabs = list
    }

    function closeTab(index) {
        var list = folderTabs
        list.splice(index, 1)
        folderTabs = list
        tabs.selectedTabIndex = 0
    }

    PageStack {
        id: pageStack

        anchors.bottomMargin: toolbar.tools.opened && toolbar.tools.locked ? -mainView.toolbar.triggerSize : 0

        Tabs {
            id: tabs

            Repeater {
                model: folderTabs
                delegate: Tab {
                    title: page.title
                    page: FolderListPage {
                        objectName: "folderPage"

                        folder: modelData
                    }
                }
            }
        }

        Component.onCompleted: {
            pageStack.push(tabs)
        }
    }

    SettingsPage {
        id: settingsPage

        visible: false
    }

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
            collapsedSidebar: false
        }
    }

    // Individual settings, used for bindings
    property bool showAdvancedFeatures: false

    property var viewMethod

    property bool collapsedSidebar: false

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
        //showAdvancedFeatures = getSetting("showAdvancedFeatures", false)
        viewMethod = getSetting("viewMethod", width > units.gu(40) ? i18n.tr("Icons") : i18n.tr("List"))
        collapsedSidebar = getSetting("collapsedSidebar", false)
    }

    Component.onCompleted: {
        reloadSettings()
    }

    function getIcon(name) {
        return /*"/usr/share/icons/ubuntu-mobile/actions/scalable/" + name + ".svg" */Qt.resolvedUrl("icons/" + name + ".png")
    }

    function error(title, message) {
        PopupUtils.open(Qt.resolvedUrl("NotifyDialog.qml"), mainView,
                        {
                            title: title,
                            text: message
                        })
    }

    Keys.onPressed: {
        print("Key pressed!")
        event.accepted = tabs.currentPage.keyPressed(event.key, event.modifiers)
    }
}
