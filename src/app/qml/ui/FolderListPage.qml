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
import Ubuntu.Components.ListItems 1.3
import org.nemomobile.folderlistmodel 1.0
import com.ubuntu.Archives 0.1
import "../components"

Page {
    id: folderListPage

    property bool helpClipboard: false

    header: PageHeader {
        title: basename(folder)
        contents: PathHistoryRow {}
        flickable: !sidebar.expanded ?
                       (folderListView.visible ? folderListView.flickable : folderIconView.flickable) : null
        leadingActionBar.actions: [
            /* Go to last folder visited */
            Action {
                id: back
                objectName: "back"
                iconName: "back"

                onTriggered: {
                    goBack()
                }
            }
        ]
        trailingActionBar {
            numberOfSlots: 3
            actions: [
                Action {
                    id: pasteButton
                    objectName: "paste"
                    iconName: "edit-paste"
                    // Translation message was implemented according to:
                    // http://developer.ubuntu.com/api/qml/sdk-14.04/Ubuntu.Components.i18n/
                    // It allows correct translation for languages with more than two plural forms:
                    // http://localization-guide.readthedocs.org/en/latest/l10n/pluralforms.html
                    text: i18n.tr("Paste %1 File", "Paste %1 Files", pageModel.clipboardUrlsCounter).arg(pageModel.clipboardUrlsCounter)
                    visible: helpClipboard // pageModel.clipboardUrlsCounter > 0
                    onTriggered: {
                        console.log("Pasting to current folder items of count " + pageModel.clipboardUrlsCounter)
                        fileOperationDialog.startOperation(i18n.tr("Paste files"))
                        pageModel.paste()
                    }
                },
                Action {
                    id: clearClipboardButton
                    objectName: "clearClipboard"
                    iconName: "edit-clear"
                    text: i18n.tr("Clear clipboard")
                    visible: helpClipboard // pageModel.clipboardUrlsCounter > 0
                    onTriggered: {
                        console.log("Clearing clipboard")
                        pageModel.clearClipboard()
                        helpClipboard = false
                    }
                },
                Action {
                    id: optionsButton
                    iconName: "view-list-symbolic"
                    text: i18n.tr("Properties")
                    onTriggered: {
                        PopupUtils.open(Qt.resolvedUrl("ViewPopover.qml"), parent)
                    }
                },
                Action {
                    id: createNewFolder
                    objectName: "createFolder"
                    iconName: "add"
                    text: i18n.tr("New Folder")
                    visible: folderListPage.__pathIsWritable
                    onTriggered: {
                        print(text)
                        PopupUtils.open(createFolderDialog, folderListPage)
                    }
                },
                Action {
                    id: viewProperties
                    iconName: "info"
                    text: i18n.tr("Properties")
                    onTriggered: {
                        print(text)
                        PopupUtils.open(Qt.resolvedUrl("FileDetailsPopover.qml"), folderListPage,{ "model": pageModel})
                    }
                },
                Action {
                    id: gotoButton
                    iconName: "find"
                    objectName:"Find"
                    text: i18n.tr("Go To")
                    visible: sidebar.expanded
                    onTriggered: PopupUtils.open(Qt.resolvedUrl("GoToDialog.qml"), parent)
                },
                Action {
                    id: unlockButton
                    iconName: "lock"
                    text: i18n.tr("Unlock full access")
                    visible: pageModel.onlyAllowedPaths
                    onTriggered: {
                        console.log("Full access clicked")
                        var authDialog = PopupUtils.open(Qt.resolvedUrl("AuthenticationDialog.qml"),
                                                         folderListPage)

                        authDialog.passwordEntered.connect(function(password) {
                            if (pamAuthentication.validatePasswordToken(password)) {
                                console.log("Authenticated for full access")
                                mainView.fullAccessGranted = true
                            } else {
                                PopupUtils.open(Qt.resolvedUrl("NotifyDialog.qml"), folderListPage,
                                                {
                                                    title: i18n.tr("Authentication failed")
                                                })

                                console.log("Could not authenticate")
                            }
                        })
                    }
                }
            ]
        }
    }

    property variant fileView: folderListPage
    property bool showingListView: folderListView.visible
    property string folder
    property bool loading: pageModel.awaitingResults
    property bool __pathIsWritable: false


    // Set to true if called as file selector for ContentHub
    property bool fileSelectorMode: fileSelectorModeG
    property bool folderSelectorMode: folderSelectorModeG
    readonly property bool selectionMode: fileSelectorMode || folderSelectorMode

    property FolderListSelection selectionManager: pageModel.selectionObject()

    readonly property bool __anchorToHeader: sidebar.expanded

    onFlickableChanged: {
        if (flickable === null) {
            folderListView.topMargin = 0
            folderIconView.flickable.topMargin = 0
        } else {
            folderListView.topMargin = units.gu(9.5)
            folderIconView.flickable.topMargin = units.gu(9.5)
        }
    }

    NetAuthenticationHandler {
        id: authenticationHandler
    }

    FolderListModel {
        id: pageModel
        path: folderListPage.folder
        enableExternalFSWatcher: true
        onlyAllowedPaths: !mainView.fullAccessGranted

        // Properties to emulate a model entry for use by FileDetailsPopover
        property bool isDir: true
        property string fileName: pathName(pageModel.path)
        property string fileSize: i18n.tr("%1 file", "%1 files", folderListView.count).arg(folderListView.count)
        property bool isReadable: true
        property bool isExecutable: true

        function checkIfIsWritable() {
            if (pageModel.path) {
                folderListPage.__pathIsWritable = pageModel.curPathIsWritable() &&
                        (!pageModel.onlyAllowedPaths || pageModel.isAllowedPath(path))
            }
        }


        Component.onCompleted: {
            // Add default allowed paths
            addAllowedDirectory(userplaces.locationDocuments)
            addAllowedDirectory(userplaces.locationDownloads)
            addAllowedDirectory(userplaces.locationMusic)
            addAllowedDirectory(userplaces.locationPictures)
            addAllowedDirectory(userplaces.locationVideos)
        }
        onNeedsAuthentication: {
            console.log("FolderListModel needsAuthentication() signal arrived")
            authenticationHandler.showDialog(urlPath,user)
        }
        onDownloadTemporaryComplete: {
            var paths = filename.split("/")
            var nameOnly = paths[paths.length -1]
            console.log("onDownloadTemporaryComplete received filename="+filename + "name="+nameOnly)
            openFromDisk(filename, nameOnly)
        }
        onOnlyAllowedPathsChanged: checkIfIsWritable()
        onPathChanged: checkIfIsWritable()


        // Following properties are set from global settings, available in filemanager.qml
        showHiddenFiles: settings.showHidden
        sortOrder: {
            switch (settings.sortOrder) {
            case 0:
                return FolderListModel.SortAscending
            case 1:
                return FolderListModel.SortDescending
            }
        }

        sortBy: {
            switch (settings.sortBy) {
            case 0:
                return FolderListModel.SortByName
            case 1:
                return FolderListModel.SortByDate
            }
        }
    }

    FolderListModel {
        id: repeaterModel
        path: folderListPage.folder

        onPathChanged: {
            console.log("Path changed to: " + repeaterModel.path)
        }
    }

    Component {
        id: createFolderDialog
        ConfirmDialogWithInput {
            title: i18n.tr("Create folder")
            text: i18n.tr("Enter name for new folder")

            onAccepted: {
                console.log("Create folder accepted", inputText)
                if (inputText !== '') {
                    var folderName = inputText.trim()
                    if (pageModel.mkdir(folderName)) {
                        folder = pageModel.path + "/" + folderName
                    }
                } else {
                    console.log("Empty directory name, ignored")
                }
            }
        }
    }

    Component {
        id: createFileDialog
        ConfirmDialogWithInput {
            title: i18n.tr("Create file")
            text: i18n.tr("Enter name for new file")

            onAccepted: {
                console.log("Create file accepted", inputText)
                if (inputText !== '') {
                    //FIXME: Actually create a new file!
                } else {
                    console.log("Empty file name, ignored")
                }
            }
        }
    }

    PlacesSidebar {
        id: sidebar
        objectName: "placesSidebar"
        anchors {
            left: mode === "left" ? parent.left : undefined
            right: mode === "right" ? parent.right : undefined
            top: folderListPage.header.bottom
            bottom: parent.bottom
        }

        expanded: showSidebar
    }

    Item {
        id: bottomBar
        anchors {
            bottom: parent.bottom
            bottomMargin: bottomEdgeTipArea + units.gu(1) // Avoid being over the bottom edge pull page
            left: sidebar.right
            right: parent.right
        }
        height: bottomBarButtons.visible ? bottomBarButtons.height : 0
        visible: bottomBarButtons.visible
    }

    Flow {
        id: bottomBarButtons
        anchors.bottom: bottomBar.bottom
        anchors.leftMargin: (parent.width - sidebar.width - childrenRect.width) / 2
        anchors.left: sidebar.right
        width: parent.width - sidebar.width
        height: units.gu(7)

        spacing: units.gu(2)
        visible: selectionMode || pageModel.onlyAllowedPaths

        function checkIfOnlyAllowed (paths) {
            var result = 0
            for (var i = 0; i < selectionManager.counter; i++)
            {
                result += (paths[i].indexOf("/home/phablet/.") !== -1) && pageModel.path === "/home/phablet"
            }
            return result === 0
        }

        Button {
            text: i18n.tr("Select")
            width: units.gu(5)
            height: units.gu(5)
            anchors.topMargin: units.gu(1)
            color: "#F5F5F5"
            iconName: "tick"
            enabled: (selectionManager.counter > 0) || (folderSelectorMode && folderListPage.__pathIsWritable)
            visible: selectionMode && isContentHub
            onClicked: {
                var selectedAbsUrls = []
                if (folderSelectorMode) {
                    selectedAbsUrls = [ folder ]
                } else {
                    var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                    // For now support only selection in filesystem
                    selectedAbsUrls = selectedAbsPaths.map(function(item) {
                        return "file://" + item;
                    });
                }
                console.log("FileSelector OK clicked, selected items: " + selectedAbsUrls)
                acceptFileSelector(selectedAbsUrls)
            }
        }
        Button {
            text: i18n.tr("Cut")
            width: units.gu(5)
            height: units.gu(5)
            anchors.topMargin: units.gu(1)
            color: "#F5F5F5"
            iconName: "edit-cut"
            enabled: ((selectionManager.counter > 0) || (folderSelectorMode && folderListPage.__pathIsWritable)) // we should discuss that: && parent.checkIfOnlyAllowed(selectionManager.selectedAbsFilePaths())
            visible: selectionMode && !isContentHub && pathIsWritable()
            onClicked: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                pageModel.cutPaths(selectedAbsPaths)
                helpClipboard = true
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
            }
        }
        Button {
            text: i18n.tr("Copy")
            width: units.gu(5)
            height: units.gu(5)
            anchors.topMargin: units.gu(1)
            color: "#F5F5F5"
            iconName: "edit-copy"
            enabled: (selectionManager.counter > 0) || (folderSelectorMode && folderListPage.__pathIsWritable)
            visible: selectionMode && !isContentHub
            onClicked: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                pageModel.copyPaths(selectedAbsPaths)
                helpClipboard = true
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
            }
        }
        Button {
            text: i18n.tr("Delete")
            width: units.gu(5)
            height: units.gu(5)
            anchors.topMargin: units.gu(1)
            color: "#F5F5F5"
            iconName: "edit-delete"
            enabled: ((selectionManager.counter > 0) || (folderSelectorMode && folderListPage.__pathIsWritable)) // we should discuss that: && parent.checkIfOnlyAllowed(selectionManager.selectedAbsFilePaths())
            visible: selectionMode && !isContentHub && pathIsWritable()
            onClicked: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                PopupUtils.open(confirmMultipleDeleteDialog, folderListPage,
                                { "paths" : selectedAbsPaths }
                                )
                selectionManager.clear()
                fileSelectorMode = false
                fileSelector.fileSelectorComponent = null
            }
        }
        Button {
            text: i18n.tr("Cancel")
            width: units.gu(5)
            height: units.gu(5)
            anchors.topMargin: units.gu(1)
            color: "#F5F5F5"
            iconName: "edit-clear"
            visible: selectionMode
            onClicked: {
                console.log("FileSelector cancelled")
                if (isContentHub)
                {
                    cancelFileSelector()
                }
                else
                {
                    selectionManager.clear()
                    fileSelectorMode = false
                    fileSelector.fileSelectorComponent = null
                }
            }
        }
    }

    // TODO: Use QML Loader for showing the right Folder*View

    FolderIconView {
        id: folderIconView

        clip: true

        folderListModel: pageModel
        anchors {
            top: __anchorToHeader ? folderListPage.header.bottom : parent.top
            bottom: bottomBar.top
            left: sidebar.right
            right: parent.right
        }
        smallMode: !sidebar.expanded
        visible: settings.viewMethod === 1  // Grid
    }

    FolderListView {
        id: folderListView

        clip: true

        folderListModel: pageModel
        anchors {
            top: __anchorToHeader ? folderListPage.header.bottom : parent.top
            bottom: bottomBar.top
            left: sidebar.right
            right: parent.right
        }
        smallMode: !sidebar.expanded
        visible: settings.viewMethod === 0  // List
    }

    function getArchiveType(fileName) {
        var splitName = fileName.split(".")

        if (splitName.length <= 1) { // To sort out files simply named "zip" or "tar"
            return ""
        }

        var fileExtension = splitName[splitName.length - 1]
        if (fileExtension === "zip") {
            return "zip"
        } else if (fileExtension === "tar") {
            return "tar"
        } else if (fileExtension === "gz") {
            if (splitName.length > 2 && splitName[splitName.length - 2] === "tar") {
                return "tar.gz"
            } else {
                return ""
            }
        } else if (fileExtension === "bz2") {
            if (splitName.length > 2 && splitName[splitName.length - 2] === "tar") {
                return "tar.bz2"
            } else {
                return ""
            }
        } else {
            return ""
        }
    }

    PlacesBottomEdge {
        id: bottomEdge

        enabled: !sidebar.expanded
        visible: enabled
    }

    Item {
        id: contents

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: sidebar.right
            right: parent.right
        }


        Label {
            text: i18n.tr("No files")
            fontSize: "large"
            opacity: 0.5
            anchors.centerIn: parent
            visible: folderListView.count == 0 && !pageModel.awaitingResults
        }

        ActivityIndicator {
            running: pageModel.awaitingResults
            width: units.gu(8)
            height: units.gu(8)
            anchors.centerIn: parent
        }
    }

    Component {
        id: confirmSingleDeleteDialog
        ConfirmDialog {
            property string filePath
            property string fileName
            title: i18n.tr("Delete")
            text: i18n.tr("Are you sure you want to permanently delete '%1'?").arg(fileName)

            onAccepted: {
                console.log("Delete accepted for filePath, fileName", filePath, fileName)

                fileOperationDialog.startOperation(i18n.tr("Deleting files"))
                console.log("Doing delete")
                pageModel.rm(filePath)
            }
        }
    }

    Component {
        id: confirmMultipleDeleteDialog
        ConfirmDialog {
            property var paths
            title: i18n.tr("Delete")
            text: i18n.tr("Are you sure you want to permanently delete '%1'?").arg(i18n.tr("these files"))

            onAccepted: {
                fileOperationDialog.startOperation(i18n.tr("Deleting files"))
                console.log("Doing delete")
                pageModel.removePaths(paths)
            }
        }
    }

    Component {
        id: confirmRenameDialog
        ConfirmDialogWithInput {
            // IMPROVE: this does not seem good: the backend expects row and new name.
            // But what if new files are added/deleted in the background while user is
            // entering the new name? The indices change and wrong file is renamed.
            // Perhaps the backend should take as parameters the "old name" and "new name"?
            // This is not currently a problem since the backend does not poll changes in
            // the filesystem, but may be a problem in the future.
            property int modelRow

            title: i18n.tr("Rename")
            text: i18n.tr("Enter a new name")

            onAccepted: {
                console.log("Rename accepted", inputText)
                if (inputText !== '') {
                    console.log("Rename commensed, modelRow/inputText", modelRow, inputText.trim())
                    if (pageModel.rename(modelRow, inputText.trim()) === false) {
                        PopupUtils.open(Qt.resolvedUrl("NotifyDialog.qml"), delegate,
                                        {
                                            title: i18n.tr("Could not rename"),
                                            text: i18n.tr("Insufficient permissions or name already exists?")
                                        }
                                        )

                    }
                } else {
                    console.log("Empty new name given, ignored")
                }
            }
        }
    }

    Component {
        id: confirmExtractDialog
        ConfirmDialog {
            property string filePath
            property string fileName
            property string archiveType
            title: i18n.tr("Extract Archive")
            text: i18n.tr("Are you sure you want to extract '%1' here?").arg(fileName)

            onAccepted: {
                extractArchive(filePath, fileName, archiveType)
            }
        }
    }

    Archives {
        id: archives
    }

    Component {
        id: actionSelectionPopoverComponent

        ActionSelectionPopover {
            id: actionSelectionPopover
            objectName: "fileActionsPopover"

            grabDismissAreaEvents: true

            property var model

            property bool isArchive: archiveType !== ""
            property string archiveType: ""

            Component.onCompleted: {
                archiveType = getArchiveType(actionSelectionPopover.model.fileName)
            }

            actions: ActionList {
                Action {
                    text: i18n.tr("Cut")
                    // TODO: temporary
                    iconSource: "image://theme/edit-cut"
                    onTriggered: {
                        console.log("Cut on row called for", actionSelectionPopover.model.fileName, actionSelectionPopover.model.index)
                        pageModel.cutIndex(actionSelectionPopover.model.index)
                        console.log("CliboardUrlsCounter after copy", pageModel.clipboardUrlsCounter)
                        helpClipboard = true
                    }
                }

                Action {
                    text: i18n.tr("Copy")
                    // TODO: temporary.
                    iconSource: "image://theme/edit-copy"

                    onTriggered: {
                        console.log("Copy on row called for", actionSelectionPopover.model.fileName, actionSelectionPopover.model.index)
                        pageModel.copyIndex(actionSelectionPopover.model.index)
                        console.log("CliboardUrlsCounter after copy", pageModel.clipboardUrlsCounter)
                        helpClipboard = true
                    }
                }

                Action {
                    text: i18n.tr("Delete")
                    // TODO: temporary
                    iconSource: "image://theme/edit-delete"
                    onTriggered: {
                        print(text)
                        PopupUtils.open(confirmSingleDeleteDialog, actionSelectionPopover.caller,
                                        { "filePath" : actionSelectionPopover.model.filePath,
                                            "fileName" : actionSelectionPopover.model.fileName }
                                        )
                    }
                }

                Action {
                    text: i18n.tr("Rename")
                    // TODO: temporary
                    iconSource: "image://theme/rotate"
                    onTriggered: {
                        print(text)
                        PopupUtils.open(confirmRenameDialog, actionSelectionPopover.caller,
                                        { "modelRow"  : actionSelectionPopover.model.index,
                                            "inputText" : actionSelectionPopover.model.fileName
                                        })
                    }
                }

                Action {
                    id: shareAction
                    text: i18n.tr("Share")
                    onTriggered: {
                        openFile(actionSelectionPopover.model, true)
                    }
                }

                Action {
                    id: extractAction
                    visible: actionSelectionPopover.isArchive
                    text: i18n.tr("Extract archive")
                    onTriggered: {
                        PopupUtils.open(confirmExtractDialog, actionSelectionPopover.caller,
                                        { "filePath" : actionSelectionPopover.model.filePath,
                                            "fileName" : actionSelectionPopover.model.fileName,
                                            "archiveType" : actionSelectionPopover.archiveType
                                        })
                    }
                }

                Action {
                    text: i18n.tr("Properties")

                    onTriggered: {
                        print(text)
                        PopupUtils.open(Qt.resolvedUrl("FileDetailsPopover.qml"),
                                        actionSelectionPopover.caller,
                                        { "model": actionSelectionPopover.model
                                        }
                                        )
                    }
                }
            }
        }
    }

    // Errors from model
    Connections {
        target: pageModel
        onError: {
            console.log("FolderListModel Error Title/Description", errorTitle, errorMessage)
            error(i18n.tr("File operation error"), errorTitle + ": " + errorMessage)
        }
    }

    FileOperationProgressDialog {
        id: fileOperationDialog

        page: folderListPage
        model: pageModel
    }

    Component {
        id: extractingDialog

        Dialog {
            id: dialog
            modal: true
            property string fileName: ""

            Row {
                id: row
                width: parent.width
                spacing: units.gu(2)

                ActivityIndicator {
                    id: loadingSpinner
                    running: true
                    anchors.verticalCenter: parent.verticalCenter
                }

                Label {
                    text: qsTr(i18n.tr("Extracting archive '%1'")).arg(fileName)
                    color: UbuntuColors.slate
                    anchors.verticalCenter: parent.verticalCenter
                    width: row.width - loadingSpinner.width - row.spacing
                    maximumLineCount: 2
                    wrapMode: Text.WrapAnywhere
                    elide: Text.ElideRight
                }
            }

            Button {
                id: cancelButton
                text: i18n.tr("Cancel")
                visible: true
                onClicked: {
                    archives.cancelArchiveExtraction()
                }
            }

            Button {
                id: okButton
                text: i18n.tr("OK")
                visible: false
                onClicked: {
                    PopupUtils.close(dialog)
                }
            }

            Connections {
                target: archives
                onFinished: {
                    if (success) {
                        PopupUtils.close(dialog)
                    } else {
                        row.visible = false
                        cancelButton.visible = false
                        title = i18n.tr("Extracting failed")
                        text = qsTr(i18n.tr("Extracting the archive '%1' failed.")).arg(fileName)
                        okButton.visible = true
                    }
                }
            }
        }
    }

    Component {
        id: openArchiveDialog

        Dialog {
            id: dialog
            modal: true
            title: i18n.tr("Archive file")
            text: i18n.tr("Do you want to extract the archive here?")
            property string filePath
            property string fileName
            property string archiveType

            Button {
                id: extractButton
                text: i18n.tr("Extract archive")
                color: UbuntuColors.green
                onClicked: {
                    PopupUtils.close(dialog)
                    extractArchive(filePath, fileName, archiveType)
                }
            }

            Button {
                id: openExternallyButton
                text: i18n.tr("Open with another app")
                color: UbuntuColors.red
                onClicked: {
                    PopupUtils.close(dialog)
                    openLocalFile(filePath)
                }
            }

            Button {
                id: cancelButton
                text: i18n.tr("Cancel")
                color: UbuntuColors.graphite
                onClicked: {
                    PopupUtils.close(dialog)
                }
            }
        }
    }

    function goTo(location) {
        // This allows us to enter "~" as a shortcut to the home folder
        // when entering a location on the Go To dialog
        folderListPage.folder = location.replace("~", userplaces.locationHome)
        refresh()
    }

    /* Go to last folder visited */
    function goBack() {
        pageModel.goBack()
        folder = pageModel.path
    }

    /* Go up one directory */
    function goUp() {
        goTo(pageModel.parentPath)
    }

    function refresh() {
        pageModel.refresh()
    }

    function pathAccessedDate() {
        console.log("calling method pageModel.curPathAccessedDate()")
        return pageModel.curPathAccessedDate()
    }

    function pathModifiedDate() {
        console.log("calling method pageModel.curPathModifiedDate()")
        return pageModel.curPathModifiedDate()
    }

    function pathIsWritable() {
        console.log("calling method pageModel.curPathIsWritable()")
        return pageModel.curPathIsWritable()
    }

    function itemDateAndSize(model) {
        var strDate = Qt.formatDateTime(model.modifiedDate, Qt.DefaultLocaleShortDate);
        //local file systems always have date and size for both files and directories
        //remote file systems may have not size for directories, it comes as "Unknown"
        if (strDate) {
            strDate += ", " + model.fileSize //show the size even it is "Unknown"
        }
        return strDate;
    }

    // FIXME: hard coded path for icon, assumes Ubuntu desktop icon available.
    // Nemo mobile has icon provider. Have to figure out what's the proper way
    // to get "system wide" icons in Ubuntu Touch, or if we have to use
    // icons packaged into the application. Both folder and individual
    // files will need an icon.

    function fileIcon(file, model) {
        var iconPath = model ? "empty" :
                               "folder"

        if (model && model.isSmbWorkgroup) {
            iconPath = "network_local"
        } else if (model && model.isHost) {
            iconPath = "server"
        } else if (model && model.isBrowsable) {
            iconPath = "folder"
        } else if (file === userplaces.locationHome) {
            iconPath = "folder-home"
        } else if (file === i18n.tr("~/Desktop")) {
            iconPath = "user-desktop"
        } else if (file === userplaces.locationDocuments) {
            iconPath = "folder-documents"
        } else if (file === userplaces.locationDownloads) {
            iconPath = "folder-downloads"
        } else if (file === userplaces.locationMusic) {
            iconPath = "folder-music"
        } else if (file === userplaces.locationPictures) {
            iconPath = "folder-pictures"
        } else if (file === i18n.tr("~/Public")) {
            iconPath = "folder-publicshare"
        } else if (file === i18n.tr("~/Programs")) {
            iconPath = "folder-system"
        } else if (file === i18n.tr("~/Templates")) {
            iconPath = "folder-templates"
        } else if (file === userplaces.locationVideos) {
            iconPath = "folder-videos"
        } else if (file === "/") {
            iconPath = "drive-harddisk"
        } else if (file === userplaces.locationSamba) {
            iconPath = "network_local"
        }  else if (userplaces.isUserMountDirectory(file)) {
            // In context of Ubuntu Touch this means SDCard currently.
            iconPath = "drive-removable-media"
        }

        return "image://theme/%1".arg(iconPath)
    }

    function folderDisplayName(folder) {
        if (folder === userplaces.locationHome) {
            return i18n.tr("Home")
        } else if (folder === "/") {
            return i18n.tr("Device")
        } else if (folder === userplaces.locationSamba) {
            return i18n.tr("Network")
        } else {
            return basename(folder)
        }
    }

    /* Return depth of current path */
    function pathModel(path){
        if (path === "/") { return 0 } // Otherwise it will return 1
        return path.split("/").length - 1
    }

    /* Return folder name by its depth in current path */
    function pathText(path,index) {
        return basename(path.split('/').slice(0,index+2).join("/"))
    }

    /* Return folder path by its depth in current path */
    function pathRaw(path,index) {
        return path.split('/').slice(0,index+2).join("/")
    }

    function pathName(folder) {
        if (folder === "/") {
            return "/"
        } else {
            return basename(folder)
        }
    }

    function basename(folder) {
        // Returns the latest component (folder) of an absolute path
        // E.g. basename('/home/phablet/Música') returns 'Música'

        // Remove the last trailing '/' if there is one

        folder.replace(/\/$/, "")
        return folder.substr(folder.lastIndexOf('/') + 1)
    }

    function pathExists(path) {
        path = path.replace("~", pageModel.homePath())

        if (path === '/')
            return true

        if(path.charAt(0) === '/') {
            console.log("Directory: " + path.substring(0, path.lastIndexOf('/')+1))
            repeaterModel.path = path.substring(0, path.lastIndexOf('/')+1)
            console.log("Sub dir: " + path.substring(path.lastIndexOf('/')+1))
            if (path.substring(path.lastIndexOf('/')+1) !== "" && !repeaterModel.cdIntoPath(path.substring(path.lastIndexOf('/')+1))) {
                return false
            } else {
                return true
            }
        } else {
            return false
        }
    }

    function openFromDisk(fullpathname, name, share) {
        console.log("openFromDisk():"+ fullpathname)
        // Check if file is an archive. If yes, ask the user whether he wants to extract it
        var archiveType = getArchiveType(name)
        if (archiveType === "") {
            openLocalFile(fullpathname, share)
        } else {
            PopupUtils.open(openArchiveDialog, folderListView,
                            {   "filePath" : fullpathname,
                                "fileName" : name,
                                "archiveType" : archiveType
                            })
        }

    }

    //High Level openFile() function
    //remote files are saved as temporary files and then opened
    function openFile(model, share) {
        if (model.isRemote) {
            //download and open later when the signal downloadTemporaryComplete() arrives
            pageModel.downloadAsTemporaryFile(model.index)
        }
        else {
            openFromDisk(model.filePath, model.fileName, share)
        }
    }

    function itemClicked(model) {
        if (model.isBrowsable) {
            console.log("browsable path="+model.filePath+" isRemote="+model.isRemote+" needsAuthentication="+model.needsAuthentication)
            if ((model.isReadable && model.isExecutable) ||
                    (model.isRemote && model.needsAuthentication) //in this case it is necessary to generate the signal needsAuthentication()
                    ) {
                console.log("Changing to dir", model.filePath)
                //prefer pageModel.cdIntoIndex() because it is not necessary to parse the path
                //goTo(model.filePath)
                folder = model.filePath
                pageModel.cdIntoIndex(model.index)
            } else {
                PopupUtils.open(Qt.resolvedUrl("NotifyDialog.qml"), delegate,
                                {
                                    title: i18n.tr("Folder not accessible"),
                                    // TRANSLATORS: this refers to a folder name
                                    text: i18n.tr("Can not access %1").arg(model.fileName)

                                })
            }
        } else {
            console.log("Non dir clicked")
            if (fileSelectorMode) {
                selectionManager.select(model.index,false,true)
            } else if (!folderSelectorMode){
                openFile(model)
            }
        }
    }

    function itemLongPress(delegate, model) {
        console.log("FolderListDelegate onPressAndHold")
        PopupUtils.open(actionSelectionPopoverComponent, delegate,
                        {
                            model: model
                        })
    }

    function keyPressed(key, modifiers) {
        if (key === Qt.Key_L && modifiers & Qt.ControlModifier) {
            PopupUtils.open(Qt.resolvedUrl("GoToDialog.qml"), mainView);
            return true;
        } else if (key === Qt.Key_Backspace) {
            goUp()
        }

        return false;
    }

    function extractArchive(filePath, fileName, archiveType) {
        console.log("Extract accepted for filePath, fileName", filePath, fileName)
        PopupUtils.open(extractingDialog, mainView, { "fileName" : fileName })
        console.log("Extracting...")

        var parentDirectory = filePath.substring(0, filePath.lastIndexOf("/"))
        var fileNameWithoutExtension = fileName.substring(0, fileName.lastIndexOf(archiveType) - 1)
        var extractDirectory = parentDirectory + "/" + fileNameWithoutExtension

        // Add numbers if the directory already exist: myfile, myfile-1, myfile-2, etc.
        while (pageModel.existsDir(extractDirectory)) {
            var i = 0
            while ("1234567890".indexOf(extractDirectory.charAt(extractDirectory.length - i - 1)) !== -1) {
                i++
            }
            if (i === 0 || extractDirectory.charAt(extractDirectory.length - i - 1) !== "-") {
                extractDirectory += "-1"
            } else {
                extractDirectory = extractDirectory.substring(0, extractDirectory.lastIndexOf("-") + 1) + (parseInt(extractDirectory.substring(extractDirectory.length - i)) + 1)
            }
        }

        pageModel.mkdir(extractDirectory) // This is needed for the tar command as the given destination has to be an already existing directory

        if (archiveType === "zip") {
            archives.extractZip(filePath, extractDirectory)
        } else if (archiveType === "tar") {
            archives.extractTar(filePath, extractDirectory)
        } else if (archiveType === "tar.gz") {
            archives.extractGzipTar(filePath, extractDirectory)
        } else if (archiveType === "tar.bz2") {
            archives.extractBzipTar(filePath, extractDirectory)
        }
    }

    function newFileUniqueName(filePath, fileName) {
        var fileBaseName = fileName.substring(0, fileName.lastIndexOf("."))
        var fileExtension = fileName.substring(fileName.lastIndexOf(".") + 1)
        var fullName = filePath + "/" + fileName
        var index = 1

        while (pageModel.existsFile(fullName)) {
            fullName = filePath + "/" + fileBaseName + "-" + index + "." + fileExtension;
            index++
        }

        return fullName.substring(fullName.lastIndexOf("/") + 1);
    }

    Component.onCompleted: {
        forceActiveFocus()
    }
}
