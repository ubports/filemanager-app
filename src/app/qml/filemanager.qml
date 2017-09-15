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
import QtQuick 2.4
import Ubuntu.Components 1.3
import org.nemomobile.folderlistmodel 1.0
import Ubuntu.Components.Popups 1.3
import U1db 1.0 as U1db
import Ubuntu.Content 1.3
import com.ubuntu.PlacesModel 0.1
import com.ubuntu.PamAuthentication 0.1

import "ui"

MainView {
    id: mainView
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "filemanager"
    applicationName: "com.ubuntu.filemanager"

    width: phone ? units.gu(40) : units.gu(100)
    height: units.gu(75)

    property alias filemanager: mainView

    property bool wideAspect: width > units.gu(50) && loaded
    property bool loaded: false

    property bool allowSidebarExpanded: width > units.gu(50)
    property bool fullAccessGranted: noAuthentication || !pamAuthentication.requireAuthentication()

    property bool isContentHub: true


    onAllowSidebarExpandedChanged: {
        if (!allowSidebarExpanded)
            saveSetting("collapsedSidebar", true)
    }

    property bool showSidebar: width >= units.gu(50)

    headerColor: "#F5F5F5"
    backgroundColor: "#F5F5F5"
    footerColor: "#F5F5F5"

    QtObject {
        id: fileSelector
        property var activeTransfer: null
        property var fileSelectorComponent: null
        property bool importMode: false
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

    function openFileSelector(selectFolderMode) {
        fileSelector.fileSelectorComponent = pageStack.push(Qt.resolvedUrl("./ui/FolderListPage.qml"), { fileSelectorMode: !selectFolderMode,
                                                                folderSelectorMode: selectFolderMode,
                                                                folder: "/home"})
    }

    function cancelFileSelector() {
        console.log("Cancel file selector")
        pageStack.pop()
        fileSelector.fileSelectorComponent = null
        fileSelector.activeTransfer.state = ContentTransfer.Aborted
    }

    function acceptFileSelector(fileUrls) {
        console.log("accept file selector " + fileUrls)
        if (fileSelector.importMode) {
            importFiles(fileSelector.activeTransfer, fileUrls[0])
        }
        else
        {
            exportFiles(fileSelector.activeTransfer, fileUrls)
        }
    }

    function openLocalFile(filePath, share) {
        pageStack.push(Qt.resolvedUrl("content-hub/FileOpener.qml"), { fileUrl: "file://" + filePath, share: share} )
    }

    function startImport(activeTransfer) {
        if (activeTransfer.state === ContentTransfer.Charged) {
            fileSelector.activeTransfer = activeTransfer
            fileSelector.importMode = true
            openFileSelector(true)
        }
    }

    function importFiles(activeTransfer, destDir) {
        var fileNames = []
        for(var i=0; i < activeTransfer.items.length; i++) {
            var item = activeTransfer.items[i]
            var uniqueName = fileSelector.fileSelectorComponent.newFileUniqueName(destDir,
                                                                                  fileSelector.fileSelectorComponent.basename(String(item.url)))
            console.log("Move file to:" + destDir + " with name: " + uniqueName)
            activeTransfer.items[i].move(destDir, uniqueName)
            fileNames.push(uniqueName)
        }
        finishImport(destDir, fileNames)
    }

    function exportFiles(activeTransfer, filesUrls) {
        var results = filesUrls.map(function(fileUrl) {
            return fileSelectorResultComponent.createObject(mainView, {"url": fileUrl})
        })

        if (activeTransfer !== null) {
            activeTransfer.items = results
            activeTransfer.state = ContentTransfer.Charged
            console.log("set activeTransfer")
        } else {
            console.log("activeTransfer null, not setting, testing code")
        }
    }


    Connections {
        target: ContentHub
        onExportRequested: {
            fileSelector.activeTransfer = transfer
            openFileSelector(false)
        }
        onImportRequested: startImport(transfer)
        onShareRequested: startImport(transfer)
    }

    PageStack {
        id: pageStack

         Component.onCompleted: {
            reloadSettings()
            pageStack.push(Qt.resolvedUrl("ui/FolderListPage.qml"), { folder: "/home" })
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

    property bool showAll

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
        showAll = getSetting("showHiddenFiles", false)
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

    function finishImport(folder, urls) {
        var count = urls.length

        pageStack.pop()
        fileSelector.fileSelectorComponent = null
        pageStack.currentPage.currentPage.folder = folder
        pageStack.currentPage.currentPage.refresh()
        PopupUtils.open(Qt.resolvedUrl("./ui/NotifyDialog.qml"), mainView,
                        {
                            title: (count === 1 ? i18n.tr("File %1").arg(urls[0]) : i18n.tr("%1 Files").arg(count)),
                            text: i18n.tr("Saved to: %1").arg(folder)
                        })
    }

    Keys.onPressed: {
        print("Key pressed!")
        event.accepted = tabs.currentPage.keyPressed(event.key, event.modifiers)
    }
}
