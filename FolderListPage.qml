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
import Ubuntu.Components.Popups 0.1
import Ubuntu.Components.ListItems 0.1
import org.nemomobile.folderlistmodel 1.0

Page {
    id: folderListPage

    title: folderName(folder)

    property variant fileView: folderListPage
    property alias model: pageModel

    property bool showHiddenFiles: false

    property bool showingListView: folderListView.visible

    onShowHiddenFilesChanged: {
        pageModel.showHiddenFiles = folderListPage.showHiddenFiles
    }

    property string sortingMethod: "Name"

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

    property bool sortAccending: true

    onSortAccendingChanged: {
        console.log("Sorting accending: " + sortAccending)

        if (sortAccending) {
            pageModel.sortOrder = FolderListModel.SortAscending
        } else {
            pageModel.sortOrder = FolderListModel.SortDescending
        }
    }

    // This stores the location using ~ to represent home
    property string folder
    property string homeFolder: "~"

    // This replaces ~ with the actual home folder, since the
    // plugin doesn't recognize the ~
    property string path: folder.replace("~", pageModel.homePath())

    function goHome() {
        goTo(folderListPage.homeFolder)
    }

    function goTo(location) {
        // Since the FolderListModel returns paths using the actual
        // home folder, this replaces with ~ before actually going
        // to the specified folder
        while (location !== '/' && location.substring(location.lastIndexOf('/')+1) === "") {
            location = location.substring(0, location.length - 1)
        }

        folderListPage.folder = location.replace(pageModel.homePath(), "~")
        refresh()
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
        file = file.replace(pageModel.homePath(), "~")
        var iconPath = isDir ? "/usr/share/icons/Humanity/places/48/folder.svg"
                             : "/usr/share/icons/Humanity/mimes/48/empty.svg"

        if (file === "~") {
            iconPath = "icons/folder-home.svg"
        } else if (file === i18n.tr("~/Desktop")) {
            iconPath = "/usr/share/icons/Humanity/places/48/user-desktop.svg"
        } else if (file === i18n.tr("~/Documents")) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-documents.svg"
        } else if (file === i18n.tr("~/Downloads")) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-downloads.svg"
        } else if (file === i18n.tr("~/Music")) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-music.svg"
        } else if (file === i18n.tr("~/Pictures")) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-pictures.svg"
        } else if (file === i18n.tr("~/Public")) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-publicshare.svg"
        } else if (file === i18n.tr("~/Programs")) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-system.svg"
        } else if (file === i18n.tr("~/Templates")) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-templates.svg"
        } else if (file === i18n.tr("~/Videos")) {
            iconPath = "/usr/share/icons/Humanity/places/48/folder-videos.svg"
        } else if (file === "/") {
            iconPath = "/usr/share/icons/Humanity/devices/48/drive-harddisk.svg"
        }

        return Qt.resolvedUrl(iconPath)
    }

    function folderName(folder) {
        folder = folder.replace(pageModel.homePath(), "~")

        if (folder === folderListPage.homeFolder) {
            return i18n.tr("Home")
        } else if (folder === "/") {
            return i18n.tr("File System")
        } else {
            return folder.substr(folder.lastIndexOf('/') + 1)
        }
    }

    function pathName(folder) {
        if (folder === "/") {
            return "/"
        } else {
            return folder.substr(folder.lastIndexOf('/') + 1)
        }
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

    property bool loading: pageModel.awaitingResults

    FolderListModel {
        id: pageModel

        path: folderListPage.path

        enableExternalFSWatcher: true

        // Properties to emulate a model entry for use by FileDetailsPopover
        property bool isDir: true
        property string fileName: pathName(pageModel.path)
        property string fileSize: (folderListView.count === 1
                                   ? i18n.tr("1 file")
                                   : i18n.tr("%1 files").arg(folderListView.count))       
        property bool isReadable: true
        property bool isExecutable: true
    }

    FolderListModel {
        id: repeaterModel
        path: folderListPage.folder

        onPathChanged: {
            console.log("Path: " + repeaterModel.path)
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
                        var newTab = tabs.insertTab(tab.index + 1, "", folderListPageComponent)
                        newTab.folder = folderListPage.folder
                    }
                }

                // The current tab can be closed as long as there is at least one tab remaining
                Action {
                    text: i18n.tr("Close this tab")
                    onTriggered: {
                        tabs.removeTab(tab.index)
                    }
                    enabled: tabs.count > 1
                }
            }
        }
    }

    Component {
        id: folderActionsPopoverComponent
        ActionSelectionPopover {
            id: folderActionsPopover
            objectName: "folderActionsPopover"

            grabDismissAreaEvents: true

            actions: ActionList {
                Action {
                    text: i18n.tr("Create New Folder")
                    onTriggered: {
                        print(text)

                        PopupUtils.open(createFolderDialog, folderListPage)
                    }
                }

                // TODO: Disabled until backend supports creating files
    //            Action {
    //                text: i18n.tr("Create New File")
    //                onTriggered: {
    //                    print(text)

    //                    PopupUtils.open(createFileDialog, root)
    //                }
    //            }

                Action {
                    text: pageModel.clipboardUrlsCounter === 0
                          ? i18n.tr("Paste")
                          : pageModel.clipboardUrlsCounter === 1
                            ? i18n.tr("Paste %1 File").arg(pageModel.clipboardUrlsCounter)
                            : i18n.tr("Paste %1 Files").arg(pageModel.clipboardUrlsCounter)
                    onTriggered: {
                        console.log("Pasting to current folder items of count " + pageModel.clipboardUrlsCounter)
                        fileOperationDialog.startOperation(i18n.tr("Paste files"))
                        pageModel.paste()
                    }

                    // FIXME: This property is depreciated and doesn't seem to work!
                    //visible: pageModel.clipboardUrlsCounter > 0

                    enabled: pageModel.clipboardUrlsCounter > 0
                }

                // TODO: Disabled until support for opening apps is added
                Action {
                    text: i18n.tr("Open in Terminal")
                    onTriggered: {
                        print(text)

                        // Is this the way it will work??
                        Qt.openUrlExternally("app://terminal")
                    }

                    enabled: showAdvancedFeatures && false
                }

                Action {
                    text: i18n.tr("Properties")
                    onTriggered: {
                        print(text)
                        PopupUtils.open(Qt.resolvedUrl("FileDetailsPopover.qml"),
                                        folderListPage,
                                            { "model": pageModel
                                            }
                                        )
                    }
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



    tools: ToolbarItems {
        id: toolbar
        locked: showToolbar
        opened: showToolbar

        onLockedChanged: opened = locked

        back: ToolbarButton {
            objectName: "up"
            text: "Up"
            iconSource: getIcon("keyboard-caps")
            enabled: folder != "/"
            onTriggered: {
                goTo(pageModel.parentPath)
            }
        }

        PathBar {
            height: units.gu(5)
            width: folderListPage.width - units.gu(31)
            visible: sidebar.expanded
        }

        Item {
            width: units.gu(1)
            height: parent.height
        }

        ToolbarButton {
            id: actionsButton
            objectName: "actions"
            text: i18n.tr("Actions")
            iconSource: getIcon("properties")

            onTriggered: {
                print(text)
                PopupUtils.open(folderActionsPopoverComponent, actionsButton)
            }
        }

        ToolbarButton {
            text: i18n.tr("View")
            iconSource: getIcon("settings")
            id: optionsButton

            onTriggered: {
                print(text)

                PopupUtils.open(Qt.resolvedUrl("ViewPopover.qml"), optionsButton)
            }
        }

        ToolbarButton {
            id: goToButton
            visible: false//sidebar.expanded
            objectName: "goTo"
            text: i18n.tr("Go To")
            iconSource: getIcon("location")
            onTriggered: {
                print(text)

                PopupUtils.open(Qt.resolvedUrl("GoToDialog.qml"), goToButton)
            }
        }

        ToolbarButton {
            id: placesButton
            visible: !sidebar.expanded
            objectName: "places"
            text: i18n.tr("Places")
            iconSource: getIcon("location")
            onTriggered: {
                print(text)

                PopupUtils.open(Qt.resolvedUrl("PlacesPopover.qml"), placesButton)
            }
        }

        ToolbarButton {
            id: tabsButton
            objectName: "tabs"
            text: i18n.tr("Tabs")
            iconSource: getIcon("browser-tabs")

            onTriggered: {
                print(text)

                PopupUtils.open(tabsPopover, tabsButton, {
                                    tab: folderListPage.parent
                                })
            }
        }
    }

    flickable: !sidebar.expanded ? folderListView.visible ? folderListView : folderIconView.flickable : null

    onFlickableChanged: {
        if (flickable === null) {
            folderListView.topMargin = 0
            folderIconView.flickable.topMargin = 0
        } else {
            folderListView.topMargin = units.gu(9.5)
            folderIconView.flickable.topMargin = units.gu(9.5)
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

    FolderIconView {
        id: folderIconView

        clip: true

        folderListModel: pageModel
        anchors {
            top: parent.top
            bottom: parent.bottom
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
            bottom: parent.bottom
            left: sidebar.right
            right: parent.right
        }
        visible: viewMethod === i18n.tr("List")
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

                fileOperationDialog.startOperation("Deleting files")
                console.log("Doing delete")
                pageModel.rm(filePath)
            }
        }
    }

    Component {
        id: confirmRenameDialog
        ConfirmDialogWithInput {
            // IMPROVE: this does not seem good: the backend excepts row and new name.
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
                                         })

                    }
                } else {
                    console.log("Empty new name given, ignored")
                }
            }
        }
    }

    Component {
        id: actionSelectionPopoverComponent

        ActionSelectionPopover {
            id: actionSelectionPopover
            objectName: "fileActionsPopover"

            grabDismissAreaEvents: true

            property var model
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

    function itemClicked(model) {
        if (model.isDir) {
            if (model.isReadable && model.isExecutable) {
                console.log("Changing to dir", model.filePath)
                goTo(model.filePath)
            } else {
                PopupUtils.open(Qt.resolvedUrl("NotifyDialog.qml"), delegate,
                                {
                                    title: i18n.tr("Folder not accessible"),
                                    text: i18n.tr("Can not access ") + model.fileName

                                 })
            }
        } else {
            console.log("Non dir clicked")
            openFile(model.fileName)
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
            PopupUtils.open(Qt.resolvedUrl("GoToDialog.qml"), goToButton)
            return true;
        }

        return false;
    }

    Component.onCompleted: forceActiveFocus()
}
