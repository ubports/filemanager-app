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
import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0
import org.nemomobile.folderlistmodel 1.0
import com.ubuntu.Archives 0.1
import "../components"
import "../upstream"

PageWithBottomEdge {
    id: folderListPage
    title: basename(folder)
    bottomEdgeTitle: i18n.tr("Places")
    bottomEdgeEnabled: !sidebar.expanded
    bottomEdgePageSource: Qt.resolvedUrl("PlacesPage.qml")

    head.contents: PathHistoryRow {}

    /* Go to last folder visited */
    head.backAction: Action {
        id: back
        objectName: "back"
        iconName: "back"

        onTriggered: {
            goBack()
        }
    }

    head.actions: [
        Action {
            id: pasteButton
            objectName: "paste"
            iconName: "edit-paste"
            // Translation message was implemented according to:
            // http://developer.ubuntu.com/api/qml/sdk-14.04/Ubuntu.Components.i18n/
            // It allows correct translation for languages with more than two plural forms:
            // http://localization-guide.readthedocs.org/en/latest/l10n/pluralforms.html
            text: i18n.tr("Paste %1 File", "Paste %1 Files", pageModel.clipboardUrlsCounter).arg(pageModel.clipboardUrlsCounter)
            visible: pageModel.clipboardUrlsCounter > 0
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
            visible: pageModel.clipboardUrlsCounter > 0
            onTriggered: {
                console.log("Clearing clipboard")
                pageModel.clearClipboard()
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
            id: settingsButton
            iconName: "settings"
            objectName: "settings"
            text: i18n.tr("Settings")
            visible: sidebar.expanded
            onTriggered: pageStack.push(settingsPage);
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
                        pageModel.onlyAllowedPaths = false
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
    flickable: !sidebar.expanded ?
                   (folderListView.visible ? folderListView : folderIconView.flickable) : null

    property variant fileView: folderListPage
    property bool showHiddenFiles: false
    property bool showingListView: folderListView.visible
    property string sortingMethod: "Name"
    property bool sortAscending: true
    property string folder
    property bool loading: pageModel.awaitingResults

    // Set to true if called as file selector for ContentHub
    property bool fileSelectorMode: false

    property FolderListSelection selectionManager: pageModel.selectionObject()

    onShowHiddenFilesChanged: {
        pageModel.showHiddenFiles = folderListPage.showHiddenFiles
    }

    onSortingMethodChanged: {
        console.log("Sorting by: " + sortingMethod)
        if (sortingMethod === "Name") {
            pageModel.sortBy = FolderListModel.SortByName
        } else if (sortingMethod === "Date") {
            pageModel.sortBy = FolderListModel.SortByDate
        } else {
            // Something fatal happened!
            console.log("ERROR: Invalid sort type:", sortingMethod)
        }
    }

    onSortAscendingChanged: {
        console.log("Sorting ascending: " + sortAscending)

        if (sortAscending) {
            pageModel.sortOrder = FolderListModel.SortAscending
        } else {
            pageModel.sortOrder = FolderListModel.SortDescending
        }
    }

    onFlickableChanged: {
        if (flickable === null) {
            folderListView.topMargin = 0
            folderIconView.flickable.topMargin = 0
        } else {
            folderListView.topMargin = units.gu(9.5)
            folderIconView.flickable.topMargin = units.gu(9.5)
        }
    }

    FolderListModel {
        id: pageModel
        path: folderListPage.folder
        enableExternalFSWatcher: true
        onlyAllowedPaths: !noAuthentication && pamAuthentication.requireAuthentication()

        // Properties to emulate a model entry for use by FileDetailsPopover
        property bool isDir: true
        property string fileName: pathName(pageModel.path)
        property string fileSize: i18n.tr("%1 file", "%1 files", folderListView.count).arg(folderListView.count)
        property bool isReadable: true
        property bool isExecutable: true

        Component.onCompleted: {
            // Add default allowed paths
            addAllowedDirectory(userplaces.locationDocuments)
            addAllowedDirectory(userplaces.locationDownloads)
            addAllowedDirectory(userplaces.locationMusic)
            addAllowedDirectory(userplaces.locationPictures)
            addAllowedDirectory(userplaces.locationVideos)
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
        id: tabsPopover
        ActionSelectionPopover {
            objectName: "tabsPopover"

            property var tab

            grabDismissAreaEvents: true

            actions: ActionList {
                Action {
                    text: i18n.tr("Open in a new tab")
                    onTriggered: {
                        openTab(folderListPage.folder)
                    }
                }

                // The current tab can be closed as long as there is at least one tab remaining
                Action {
                    text: i18n.tr("Close this tab")
                    onTriggered: {
                        closeTab(tab.index)
                    }
                    enabled: tabs.count > 1
                }
            }
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
                    pageModel.mkdir(inputText)
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

        //        anchors {
        //            top: parent.top
        //            bottom: parent.bottom
        //            bottomMargin: units.gu(-2)
        //        }

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

        spacing: units.gu(2)
        visible: fileSelectorMode || pageModel.onlyAllowedPaths

        Button {
            text: i18n.tr("Select")
            enabled: selectionManager.counter > 0
            visible: fileSelectorMode
            onClicked: {
                var selectedAbsPaths = selectionManager.selectedAbsFilePaths();
                // For now support only selection in filesystem
                var selectedAbsUrls = selectedAbsPaths.map(function(item) {
                    return "file://" + item;
                });
                console.log("FileSelector OK clicked, selected items: " + selectedAbsUrls)

                acceptFileSelector(selectedAbsUrls)
            }
        }
        Button {
            text: i18n.tr("Cancel")
            visible: fileSelectorMode
            onClicked: {
                console.log("FileSelector cancelled")
                cancelFileSelector()
            }
        }
    }

    FolderIconView {
        id: folderIconView

        clip: true

        folderListModel: pageModel
        anchors {
            top: parent.top
            bottom: bottomBar.top
            left: sidebar.right
            right: parent.right
        }
        smallMode: !sidebar.expanded
        visible: viewMethod === i18n.tr("Icons")
    }

    FolderListView {
        id: folderListView

        clip: true

        folderListModel: pageModel
        anchors {
            top: parent.top
            bottom: bottomBar.top
            left: sidebar.right
            right: parent.right
        }
        smallMode: !sidebar.expanded
        visible: viewMethod === i18n.tr("List")
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
                    console.log("Rename commensed, modelRow/inputText", modelRow, inputText)
                    if (pageModel.rename(modelRow, inputText) === false) {
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

            delegate: Empty { // NOTE: This is a workaround for LP: #1395118 and should be removed as soon as the patch for upstream gets released (https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1395118)
                id: listItem
                Label {
                    text: listItem.text
                    anchors {
                        verticalCenter: parent.verticalCenter
                        horizontalCenter: parent.horizontalCenter
                    }
                    wrapMode: Text.Wrap
                    color: Theme.palette.normal.overlayText
                }
                /*! \internal */
                onTriggered: actionSelectionPopover.hide()
                visible: enabled && ((action === undefined) || action.visible)
                height: visible ? implicitHeight : 0
            }

            actions: ActionList {
                Action {
                    text: i18n.tr("Cut")
                    // TODO: temporary
                    iconSource: "/usr/share/icons/Humanity/actions/48/edit-cut.svg"
                    onTriggered: {
                        console.log("Cut on row called for", actionSelectionPopover.model.fileName, actionSelectionPopover.model.index)
                        pageModel.cutIndex(actionSelectionPopover.model.index)
                        console.log("CliboardUrlsCounter after copy", pageModel.clipboardUrlsCounter)
                    }
                }

                Action {
                    text: i18n.tr("Copy")
                    // TODO: temporary.
                    iconSource: "/usr/share/icons/Humanity/actions/48/edit-copy.svg"

                    onTriggered: {
                        console.log("Copy on row called for", actionSelectionPopover.model.fileName, actionSelectionPopover.model.index)
                        pageModel.copyIndex(actionSelectionPopover.model.index)
                        console.log("CliboardUrlsCounter after copy", pageModel.clipboardUrlsCounter)
                    }
                }

                Action {
                    text: i18n.tr("Delete")
                    // TODO: temporary
                    iconSource: "/usr/share/icons/Humanity/actions/48/edit-delete.svg"
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
                    iconSource: "/usr/share/icons/Humanity/actions/48/rotate.svg"
                    onTriggered: {
                        print(text)
                        PopupUtils.open(confirmRenameDialog, actionSelectionPopover.caller,
                                        { "modelRow"  : actionSelectionPopover.model.index,
                                            "inputText" : actionSelectionPopover.model.fileName
                                        })
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
                    color: UbuntuColors.darkGrey
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
                    openFile(filePath)
                }
            }

            Button {
                id: cancelButton
                text: i18n.tr("Cancel")
                color: UbuntuColors.lightGrey
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

    // FIXME: hard coded path for icon, assumes Ubuntu desktop icon available.
    // Nemo mobile has icon provider. Have to figure out what's the proper way
    // to get "system wide" icons in Ubuntu Touch, or if we have to use
    // icons packaged into the application. Both folder and individual
    // files will need an icon.
    // TODO: Remove isDir parameter and use new model functions
    function fileIcon(file, isDir) {
        var iconPath = isDir ? "/usr/share/icons/Humanity/places/48/folder.svg"
                             : "/usr/share/icons/Humanity/mimes/48/empty.svg"

        if (file === userplaces.locationHome) {
            iconPath = "../icons/folder-home.svg"
        } else if (file === i18n.tr("~/Desktop")) {
            iconPath = "/usr/share/icons/Humanity/places/48/user-desktop.svg"
        } else if (file === userplaces.locationDocuments) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-documents.svg"
        } else if (file === userplaces.locationDownloads) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-downloads.svg"
        } else if (file === userplaces.locationMusic) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-music.svg"
        } else if (file === userplaces.locationPictures) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-pictures.svg"
        } else if (file === i18n.tr("~/Public")) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-publicshare.svg"
        } else if (file === i18n.tr("~/Programs")) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-system.svg"
        } else if (file === i18n.tr("~/Templates")) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-templates.svg"
        } else if (file === userplaces.locationVideos) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-videos.svg"
        } else if (file === "/") {
            iconPath = "/usr/share/icons/Humanity/devices/48/drive-harddisk.svg"
        } else if (userplaces.isUserMountDirectory(file)) {
            // In context of Ubuntu Touch this means SDCard currently.
            iconPath = "/usr/share/icons/Humanity/devices/48/drive-removable-media.svg"
        }

        return Qt.resolvedUrl(iconPath)
    }

    function folderDisplayName(folder) {
        if (folder === userplaces.locationHome) {
            return i18n.tr("Home")
        } else if (folder === "/") {
            return i18n.tr("Device")
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

    function itemClicked(model) {
        if (model.isBrowsable) {
            if (model.isReadable && model.isExecutable) {
                console.log("Changing to dir", model.filePath)
                goTo(model.filePath)
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
                selectionManager.select(model.index,
                                        false,
                                        true);
            } else {
                // Check if file is an archive. If yes, ask the user whether he wants to extract it
                var archiveType = getArchiveType(model.fileName)
                if (archiveType === "") {
                    openFile(model.filePath)
                } else {
                    PopupUtils.open(openArchiveDialog, folderListView,
                                    { "filePath" : model.filePath,
                                        "fileName" : model.fileName,
                                        "archiveType" : archiveType
                                    })
                }

            }
            //            PopupUtils.open(Qt.resolvedUrl("FileActionDialog.qml"), root,
            //                            {
            //                                fileName: model.fileName,
            //                                filePath: model.filePath,
            //                                folderListModel: root.folderListModel
            //                            })
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

    Component.onCompleted: {
        forceActiveFocus()
    }
}
