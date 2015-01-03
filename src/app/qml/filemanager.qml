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
import QtQuick 2.3
import Ubuntu.Components 1.1
import org.nemomobile.folderlistmodel 1.0
import Ubuntu.Components.Popups 1.0
import Ubuntu.Unity.Action 1.1 as UnityActions
import U1db 1.0 as U1db
import Ubuntu.Content 0.1
import com.ubuntu.PlacesModel 0.1
import com.ubuntu.PamAuthentication 0.1

import "ui"

MainView {
    id: mainView
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "filemanager"
    applicationName: "com.ubuntu.filemanager"
    useDeprecatedToolbar: false

    width: phone ? units.gu(40) : units.gu(100)
    height: units.gu(75)

    property alias filemanager: mainView

    property bool wideAspect: width > units.gu(50) && loaded
    property bool loaded: false

    property bool allowSidebarExpanded: width > units.gu(50)

    onAllowSidebarExpandedChanged: {
        if (!allowSidebarExpanded)
            saveSetting("collapsedSidebar", true)
    }

    property bool showSidebar: width >= units.gu(50)

    headerColor: "#464646"
    backgroundColor: "#464646"
    footerColor: "#464646"

    QtObject {
        id: fileSelector
        property var activeTransfer: null
        property var fileSelectorComponent: null
    }

    Component {
        id: fileSelectorResultComponent
        ContentItem {}
    }

    PlacesModel {
       id: userplaces
    }

    PamAuthentication {
        id: pamAuthentication
        serviceName: "filemanager"
    }

    // HUD Actions
    Action {
        id: settingsAction
        text: i18n.tr("Settings")
        description: i18n.tr("Change app settings")
        iconSource: getIcon("settings")
        onTriggered: showSettings()
    }
    actions: [settingsAction]

    property var pageStack: pageStack

    property var folderTabs: [userplaces.locationHome]

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

    function openFileSelector() {
        pageStack.push(fileSelectorComponent, { fileSelectorMode: true} )
    }

    function cancelFileSelector() {
        console.log("Cancel file selector")
        pageStack.pop()
        fileSelector.fileSelectorComponent = null
        fileSelector.activeTransfer.state = ContentTransfer.Aborted
    }

    function acceptFileSelector(fileUrls) {
        console.log("accept file selector " + fileUrls)
        var results = fileUrls.map(function(fileUrl) {
            return fileSelectorResultComponent.createObject(mainView, {"url": fileUrl})
        })

        if (fileSelector.activeTransfer !== null) {
            fileSelector.activeTransfer.items = results
            fileSelector.activeTransfer.state = ContentTransfer.Charged
            console.log("set activeTransfer")
        } else {
            console.log("activeTransfer null, not setting, testing code")
        }
    }

    function openFile(filePath) {
        pageStack.push(Qt.resolvedUrl("content-hub/FileOpener.qml"), { fileUrl: "file://" + filePath} )
    }

    Connections {
        target: ContentHub
        onExportRequested: {
            fileSelector.activeTransfer = transfer
            openFileSelector()
        }
    }

    Component {
        id: fileSelectorComponent

        FolderListPage {
            // TODO: remember last selection
            folder: userplaces.locationHome
        }
    }

    PageStack {
        id: pageStack

        Tabs {
            id: tabs

            Tab {
                title: page.title
                page: FolderListPage {
                    objectName: "folderPage"

                    folder: userplaces.locationHome //modelData
                }
            }
            Tab {
                title: "page.title"
                page: Page {
                    objectName: "settingsPage"
                }
            }
            Tab {
                title: "page.title"
                page: SettingsSheet {
                    id: settingsPage
                }
            }


            // TODO: Temporarily disabled tabs support since this is broken in the SDK (lp:1295242)
//            Repeater {
//                model: folderTabs
//                delegate: Tab {
//                    title: page.title
//                    page: FolderListPage {
//                        objectName: "folderPage"

//                        folder: modelData
//                    }
//                }
//            }
        }

        Component.onCompleted: {
            pageStack.push(tabs)
            pageStack.push(Qt.resolvedUrl("ui/FolderListPage.qml"))
            pageStack.pop()
            loaded = true
        }
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

    function showSettings() {
        PopupUtils.open(Qt.resolvedUrl("ui/SettingsSheet.qml"), mainView)
    }

    function reloadSettings() {
        //showAdvancedFeatures = getSetting("showAdvancedFeatures", false)
        viewMethod = getSetting("viewMethod", wideAspect ? i18n.tr("Icons") : i18n.tr("List"))
        collapsedSidebar = getSetting("collapsedSidebar", false)
    }

    Component.onCompleted: {
        reloadSettings()
    }

    function getIcon(name) {
        return "/usr/share/icons/ubuntu-mobile/actions/scalable/" + name + ".svg" //Qt.resolvedUrl("icons/" + name + ".png")
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
