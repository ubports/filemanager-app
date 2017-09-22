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
import Ubuntu.Components.Popups 1.3
import Qt.labs.settings 1.0
import Ubuntu.Content 1.3
import com.ubuntu.PamAuthentication 0.1

// This makes the Ubuntu Thumbnailer available in all the other QML documents.
import Ubuntu.Thumbnailer 0.1

import "ui"
import "backend" as Backend

MainView {
    id: mainView
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "filemanager"
    applicationName: "com.ubuntu.filemanager"

    width: phone ? units.gu(40) : units.gu(100)
    height: units.gu(75)

    property bool wideAspect: width > units.gu(50)

    property bool fullAccessGranted: noAuthentication || !pamAuthentication.requireAuthentication()
    property bool isContentHub: false
    property bool importMode: true

    QtObject {
        id: fileSelector
        property var activeTransfer: null
        property var fileSelectorComponent: null
    }

    Component {
        id: fileSelectorResultComponent
        ContentItem {}
    }

    PamAuthentication {
        id: pamAuthentication
        serviceName: "filemanager"
    }

    property var pageStack: pageStack

    function openFileSelector(selectFolderMode) {
        var props = {
            fileSelectorMode: !selectFolderMode,
            folderSelectorMode: selectFolderMode
        }
        fileSelector.fileSelectorComponent = pageStack.push(Qt.resolvedUrl("./ui/FolderListPage.qml"), props)
    }

    function cancelFileSelector() {
        console.log("Cancel file selector")
        pageStack.pop()
        fileSelector.fileSelectorComponent = null
        fileSelector.activeTransfer.state = ContentTransfer.Aborted
    }

    function acceptFileSelector(fileUrls) {
        console.log("accept file selector " + fileUrls)
        if (importMode) {
            importFiles(fileSelector.activeTransfer, fileUrls[0])
        } else {
            exportFiles(fileSelector.activeTransfer, fileUrls)
        }
    }

    function openLocalFile(filePath, share) {
        pageStack.push(Qt.resolvedUrl("content-hub/FileOpener.qml"), { fileUrl: "file://" + filePath, share: share} )
    }

    function startTransfer(activeTransfer, iMode) {
        if (activeTransfer.state === ContentTransfer.Charged || !iMode) {
            fileSelector.activeTransfer = activeTransfer
            isContentHub = true
            importMode = iMode
            openFileSelector(iMode)
        }
    }

    function importFiles(activeTransfer, destDir) {
        var fileNames = []
        for(var i=0; i < activeTransfer.items.length; i++) {
            var item = activeTransfer.items[i]
            var uniqueName = fileSelector.fileSelectorComponent.folderModel.newFileUniqueName(destDir,
                                                                                  fileSelector.fileSelectorComponent.folderModel.basename(String(item.url)))
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
        onExportRequested: startTransfer(transfer, false)
        onImportRequested: startTransfer(transfer, true)
        onShareRequested: startTransfer(transfer, true)
    }

    PageStack {
        id: pageStack
    }

    /* Settings Storage */
    property QtObject globalSettings: Backend.GlobalSettings { }


    function error(title, message) {
        var props = {
            title: title,
            text: message
        }

        PopupUtils.open(Qt.resolvedUrl("dialogs/NotifyDialog.qml"), mainView, props)
    }

    function finishImport(folder, urls) {
        var count = urls.length

        pageStack.pop()
        fileSelector.fileSelectorComponent = null
        pageStack.currentPage.folderModel.path = folder
        pageStack.currentPage.refresh()

        var props = {
            title: (count === 1 ? i18n.tr("File %1").arg(urls[0]) : i18n.tr("%1 Files").arg(count)),
            text: i18n.tr("Saved to: %1").arg(folder)
        }

        PopupUtils.open(Qt.resolvedUrl("dialogs/NotifyDialog.qml"), mainView, props)
    }

    Component.onCompleted:  {
        QuickUtils.mouseAttached = true
        pageStack.push(Qt.resolvedUrl("ui/FolderListPage.qml"))
    }
}
