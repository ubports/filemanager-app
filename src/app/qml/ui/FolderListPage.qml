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
 *              Niklas Wenzel <nikwen.developer@gmail.com>
 */
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import org.nemomobile.folderlistmodel 1.0

import "../components"
import "../actions" as FMActions
import "../dialogs" as Dialogs
import "../backend" as Backend

SidebarPageLayout {
    id: folderListPage

    property alias folderModel: pageModel
    Backend.FolderListModel {
        id: pageModel
        path: places.locationHome

        model.onlyAllowedPaths: !mainView.fullAccessGranted
        model.onNeedsAuthentication: {
            console.log("FolderListModel needsAuthentication() signal arrived")
            authenticationHandler.showDialog(urlPath, user)
        }

        model.onDownloadTemporaryComplete: {
            var paths = filename.split("/")
            var nameOnly = paths[paths.length -1]
            console.log("onDownloadTemporaryComplete received filename="+filename + "name="+nameOnly)
            openFromDisk(filename, nameOnly)
        }

        // Following properties are set from global settings, available in filemanager.qml
        model.showHiddenFiles: globalSettings.showHidden
        model.sortOrder: {
            switch (globalSettings.sortOrder) {
            case 0: return FolderListModel.SortAscending
            case 1: return FolderListModel.SortDescending
            }
        }

        model.sortBy: {
            switch (globalSettings.sortBy) {
            case 0: return FolderListModel.SortByName
            case 1: return FolderListModel.SortByDate
            }
        }
    }

    sidebarWidth: globalSettings.sidebarWidth
    onSidebarWidthChanged: {
        if (sidebarWidth > sidebarMaximumWidth)
            globalSettings.sidebarWidth = sidebarMaximumWidth
        else if (sidebarWidth < sidebarMinimumWidth)
            globalSettings.sidebarWidth = sidebarMinimumWidth
        else
            globalSettings.sidebarWidth = folderListPage.sidebarWidth
    }

    Binding {
        when: !sidebarResizing
        target: folderListPage
        property: "sidebarWidth"
        value: globalSettings.sidebarWidth
    }

    sidebarActive: mainView.wideAspect
    sidebarLoader.sourceComponent: PlacesPage {
        anchors.fill: parent
        folderModel: pageModel
    }

    mainLoader.sourceComponent: Page {
        id: folderPage

        header: FolderListPageDefaultHeader {
            fileOperationDialog: fileOperationDialogObj
            folderModel: pageModel
            showPanelAction: folderListPage.showPanelAction
        }

        // Set to true if called as file selector for ContentHub
        property bool fileSelectorMode: fileSelectorModeG
        property bool folderSelectorMode: folderSelectorModeG
        readonly property bool selectionMode: fileSelectorMode || folderSelectorMode

        NetAuthenticationHandler {
            id: authenticationHandler
            folderListModel: pageModel.model
        }

        Loader {
            id: viewLoader
            anchors.fill: parent
            anchors.topMargin: folderPage.header.height
            anchors.bottomMargin: selectionBottomBar.height

            sourceComponent: {
                if (globalSettings.viewMethod === 1) { // Grid
                    return folderIconView
                } else {
                    return folderListView
                }
            }
        }

        // TODO: ClipboardBottomBar
        // Should stay here, so it goes below SelectionBottomBar if visible

        SelectionBottomBar {
            id: selectionBottomBar

            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            folderModel: pageModel
            visible: selectionMode || pageModel.model.onlyAllowedPaths
        }

        // TODO: Create another bottom panel, only for ContentHub actions


        // *** VIEW COMPONENTS ***

        Component {
            id: folderIconView
            FolderIconView {
                anchors.fill: parent
                folderModel: pageModel
                folderListPage: folderPage
                fileOperationDialog: fileOperationDialogObj
            }
        }

        Component {
            id: folderListView
            FolderListView {
                anchors.fill: parent
                folderModel: pageModel
                folderListPage: folderPage
                fileOperationDialog: fileOperationDialogObj
            }
        }

        Item {
            id: contents
            anchors.fill: parent

            Label {
                text: i18n.tr("No files")
                fontSize: "large"
                opacity: 0.5
                anchors.centerIn: parent
                visible: folderListView.count == 0 && !pageModel.busy
            }

            ActivityIndicator {
                running: pageModel.busy
                anchors.centerIn: parent
            }
        }

        // Errors from model
        Connections {
            target: pageModel.model
            onError: {
                console.log("FolderListModel Error Title/Description", errorTitle, errorMessage)
                error(i18n.tr("File operation error"), errorTitle + ": " + errorMessage)
            }
        }

        Dialogs.FileOperationProgressDialog {
            id: fileOperationDialogObj

            page: folderPage
            model: pageModel.model
        }

        function openFromDisk(fullpathname, name, share) {
            console.log("openFromDisk():"+ fullpathname)
            // Check if file is an archive. If yes, ask the user whether he wants to extract it
            var archiveType = pageModel.getArchiveType(name)
            if (archiveType === "") {
                openLocalFile(fullpathname, share)
            } else {
                var props = {
                    "filePath" : fullpathname,
                    "fileName" : name,
                    "archiveType" : archiveType,
                    "folderListPage" : folderPage,
                    "folderModel": pageModel
                }
                PopupUtils.open(Qt.resolvedUrl("../dialogs/OpenArchiveDialog.qml"), folderListView, props)
            }

        }

        //High Level openFile() function
        //remote files are saved as temporary files and then opened
        function openFile(model, share) {
            if (model.isRemote) {
                //download and open later when the signal downloadTemporaryComplete() arrives
                pageModel.model.downloadAsTemporaryFile(model.index)
            }
            else {
                openFromDisk(model.filePath, model.fileName, share)
            }
        }

        Component.onCompleted: {
            forceActiveFocus()
        }
    }

    BottomEdge {
        id: bottomEdge

        hint {
            iconName: "location"
            text: i18n.tr("Places")
            enabled: visible
            visible: bottomEdge.enabled
        }

        contentComponent: PlacesPage {
            width: bottomEdge.width
            height: bottomEdge.height
            folderModel: pageModel
            onPathClicked: bottomEdge.collapse()
        }

        enabled: visible
        visible: !folderListPage.sidebarActive /* && !selectionBottomBar.visible*/
    }
}
