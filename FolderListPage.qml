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

    property bool showHiddenFiles: false

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

            delegate: Empty {
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
                onTriggered: folderActionsPopover.hide()
                visible: listItem.enabled
                height: visible ? implicitHeight : 0
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
        locked: wideAspect
        opened: wideAspect

        onLockedChanged: opened = Qt.binding(function() { return wideAspect })

        back: ToolbarButton {
            objectName: "up"
            text: "Up"
            iconSource: getIcon("up")
            visible: folder != "/"
            onTriggered: {
                goTo(pageModel.parentPath)
            }
        }

        ToolbarButton {
            id: actionsButton
            objectName: "actions"
            text: i18n.tr("Actions")
            iconSource: getIcon("edit")

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
            visible: wideAspect
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
            visible: !wideAspect
            objectName: "places"
            text: i18n.tr("Places")
            iconSource: getIcon("location")
            onTriggered: {
                print(text)

                PopupUtils.open(Qt.resolvedUrl("PlacesPopover.qml"), placesButton)
            }
        }
    }

    flickable: !wideAspect ? folderListView : null

    onFlickableChanged: {
        if (wideAspect) {
            folderListView.topMargin = 0
        } else {
            folderListView.topMargin = units.gu(9.5)
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

        expanded: wideAspect
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
    }

    Item {
        id: contents

        anchors {
            top: parent.top
            bottom: parent.bottom
            left: sidebar.right
            right: parent.right
        }

        Column {
            anchors.centerIn: parent
            Label {
                text: i18n.tr("No files")
                fontSize: "large"
                visible: folderListView.count == 0 && !pageModel.awaitingResults
            }
            ActivityIndicator {
                running: pageModel.awaitingResults
                width: units.gu(8)
                height: units.gu(8)
            }
        }
    }

    // Errors from model
    Connections {
        target: pageModel
        onError: {
            console.log("FolderListModel Error Title/Description", errorTitle, errorMessage)
            PopupUtils.open(Qt.resolvedUrl("NotifyDialog.qml"), folderListPage,
                            {
                                // Unfortunately title can not handle too long texts. TODO: bug report
                                // title: i18n.tr(errorTitle),
                                title: i18n.tr("File operation error"),
                                text: errorTitle + ": " + errorMessage
                            })
        }
    }

    FileOperationProgressDialog {
        id: fileOperationDialog

        page: folderListPage
        model: pageModel
    }
}
