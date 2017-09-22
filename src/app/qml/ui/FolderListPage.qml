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

// TODO: Review last position code, which is referenced in FolderListModel (backend), FolderDelegateActions, FolderIconView, FolderListView, FolderListPageDefaultHeader, (PlacesPage)

SidebarPageLayout {
    id: folderListPage

    property alias folderModel: pageModel
    property bool fileSelectorMode: false
    property bool folderSelectorMode: false
    property bool search: false
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

        onPathChanged: pageModel.model.selectionObject.clear()
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

        // *** HEADERS ***

        header: defaultHeader

        FolderListPageDefaultHeader {
            id: defaultHeader
            fileOperationDialog: fileOperationDialogObj
            folderModel: pageModel
            showPanelAction: folderListPage.showPanelAction
            visible: !selectionMode && !search
            enabled: visible
        }

        FolderListPageSelectionHeader {
            id: selectionHeader
            folderModel: pageModel
            visible: selectionMode && !isContentHub
            enabled: visible
        }

        FolderListPagePickModeHeader {
            id: pickModeHeader
            folderModel: pageModel
            visible: selectionMode && isContentHub
            enabled: visible
        }

        FolderListPageSearchHeader {
            id: searchHeader
            folderModel: pageModel
            visible: !selectionMode && search
            enabled: visible
        }

        // FIXME: Clearing selection (by cancel btn in the header, or changing the folder, should exit selection mode)
        readonly property bool selectionMode: fileSelectorMode || folderSelectorMode

        NetAuthenticationHandler {
            id: authenticationHandler
            folderListModel: pageModel.model
        }

        Loader {
            id: viewLoader
            anchors.fill: parent
            anchors.topMargin: folderPage.header.height
            anchors.bottomMargin: (bottomEdge.enabled ? bottomEdge.hint.height : 0) + (selectionBottomBar.visible ? selectionBottomBar.height
                                                                                                                  : clipboardBottomBar.visible ? clipboardBottomBar.height
                                                                                                                                               : 0)

            sourceComponent: {
                if (globalSettings.viewMethod === 1) { // Grid
                    return folderIconView
                } else {
                    return folderListView
                }
            }
        }

        ClipboardBottomBar {
            id: clipboardBottomBar

            anchors {
                bottom: parent.bottom; bottomMargin: bottomEdge.enabled ? bottomEdge.hint.height : 0
                left: parent.left
                right: parent.right
            }

            folderModel: pageModel
            fileOperationDialog: fileOperationDialogObj
            visible: pageModel.model.clipboardUrlsCounter > 0 && !selectionMode
        }

        SelectionBottomBar {
            id: selectionBottomBar

            anchors {
                bottom: parent.bottom   // No bottom margin here, since BottomEdge is disabled during selection.
                left: parent.left
                right: parent.right
            }

            folderModel: pageModel
            fileOperationDialog: fileOperationDialogObj
            visible: selectionMode
        }

        // *** VIEW COMPONENTS ***

        Component {
            id: folderIconView
            FolderIconView {
                anchors.fill: parent
                folderModel: pageModel
                folderListPage: folderPage
                fileOperationDialog: fileOperationDialogObj
                header: pageModel.count > 0 && !folderModel.model.isCurAllowedPath && folderModel.model.onlyAllowedPaths
                        ? authReqHeader
                        : null
            }
        }

        Component {
            id: folderListView
            FolderListView {
                anchors.fill: parent
                folderModel: pageModel
                folderListPage: folderPage
                fileOperationDialog: fileOperationDialogObj
                header: pageModel.count > 0 && !folderModel.model.isCurAllowedPath && folderModel.model.onlyAllowedPaths
                        ? authReqHeader
                        : null
            }
        }

        FMActions.UnlockFullAccess {
            id: authAction
            onTriggered: {
                console.log("Full access clicked")
                var authDialog = PopupUtils.open(Qt.resolvedUrl("../dialogs/AuthenticationDialog.qml"), mainView)

                authDialog.passwordEntered.connect(function(password) {
                    if (pamAuthentication.validatePasswordToken(password)) {
                        console.log("Authenticated for full access")
                        mainView.fullAccessGranted = true
                    } else {
                        var props = { title: i18n.tr("Authentication failed") }
                        PopupUtils.open(Qt.resolvedUrl("../dialogs/NotifyDialog.qml"), mainView, props)

                        console.log("Could not authenticate")
                    }
                })
            }
        }

        Component {
            id: authReqHeader
            ListItem {
                anchors { left: parent.left; right: parent.right }
                divider.visible: false
                height: layout.height
                ListItemLayout {
                    id: layout
                    title.text: i18n.tr("Restricted access")
                    subtitle.text: i18n.tr("Authentication is required in order to see all the content of this folder.")
                    Button {
                        width: units.gu(16)
                        SlotsLayout.position: SlotsLayout.Last
                        color: UbuntuColors.green
                        action: authAction
                    }
                }
            }
        }

        Loader {
            id: emptyStateLoader
            anchors.fill: parent
            sourceComponent: {
                if (folderModel.count == 0 && !folderModel.model.isCurAllowedPath && folderModel.model.onlyAllowedPaths)
                    return authEmptyState

                if (folderModel.count == 0 && !folderModel.awaitingResults)
                    return noFilesEmptyState
            }

            ActivityIndicator {
                anchors.centerIn: parent
                running: folderModel.model.awaitingResults
            }
        }

        Component {
            id: noFilesEmptyState

            Item {
                anchors.fill: parent
                EmptyState {
                    anchors.centerIn: parent
                    iconName: "document-open"
                    title: i18n.tr("No files")
                    subTitle: i18n.tr("This folder is empty.")
                }
            }
        }

        Component {
            id: authEmptyState

            Item {
                anchors.fill: parent
                EmptyState {
                    anchors.centerIn: parent
                    iconName: "lock"
                    title: i18n.tr("Restricted access")
                    subTitle: i18n.tr("Authentication is required in order to see the content of this folder.")

                    controlComponent: Button {
                        width: units.gu(24)
                        color: UbuntuColors.green
                        action: authAction
                    }
                }
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
                PopupUtils.open(Qt.resolvedUrl("../dialogs/OpenArchiveDialog.qml"), mainView, props)
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
        visible: !folderListPage.sidebarActive  && !mainLoader.item.selectionMode
    }
}
