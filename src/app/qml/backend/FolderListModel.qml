/*
* This file is part of Liri.
 *
* Copyright (C) 2015 Michael Spencer <sonrisesoftware@gmail.com>
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import com.ubuntu.Archives 0.1
import org.nemomobile.folderlistmodel 1.0
import com.ubuntu.PlacesModel 0.1

QtObject {
    id: folderModel

    property alias path: __model.path
    property string title: FmUtils.pathName(path)
    property string folder: FmUtils.pathName(path)

    property alias count: __model.count

    property alias busy: __model.awaitingResults
    property alias canGoBack: __model.canGoBack

    property bool showHiddenFiles: false
    property string sortingMethod: "Name" // or "Date"
    property bool sortAscending: true

    property alias model: __model
    property alias places: __places
    property alias archives: __archives

    function goTo(location) {
        // This allows us to enter "~" as a shortcut to the home folder
        // when entering a location on the Go To dialog
        path = location.replace("~", places.locationHome)

        refresh()
    }

    /* Go to last folder visited */
    function goBack() {
        model.goBack()
        path = model.path
    }

    /* Go up one directory */
    function goUp() {
        goTo(model.parentPath)
    }

    function refresh() {
        model.refresh()
    }

    function getArchiveType(fileName) {
        var splitName = fileName.split(".")
        var fileExtension = splitName[splitName.length - 1]
        if (fileExtension === "zip") {
            return "zip"
        } else if (fileExtension === "tar") {
            return "tar"
        } else {
            return ""
        }
    }

    function extractArchive(filePath, fileName, archiveType) {
        console.log("Extract accepted for filePath, fileName", filePath, fileName)
        PopupUtils.open(Qt.resolvedUrl("../dialogs/ExtractingDialog.qml"), mainView, { "fileName" : fileName, "archives": archives })
        console.log("Extracting...")

        var parentDirectory = filePath.substring(0, filePath.lastIndexOf("/"))
        var fileNameWithoutExtension = fileName.substring(0, fileName.lastIndexOf(archiveType) - 1)
        var extractDirectory = parentDirectory + "/" + fileNameWithoutExtension

        // Add numbers if the directory already exist: myfile, myfile-1, myfile-2, etc.
        while (folderModel.model.existsDir(extractDirectory)) {
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

        folderModel.model.mkdir(extractDirectory) // This is needed for the tar command as the given destination has to be an already existing directory

        archives.extract(filePath, extractDirectory)
    }

    function setPathWithAuthentication(currentPath, currentUserName, currentPassword, savePassword) {
        __model.setPathWithAuthentication(currentPath, currentUserName, currentPassword, savePassword)
    }

    // TODO: Set onlyAllowedPaths for restricted user accounts
    property QtObject __model: FolderListModel {
        id: __model

        enableExternalFSWatcher: true

        // Properties to emulate a model entry for use by FileDetailsPopover
        property bool isDir: true
        property string fileName: FmUtils.pathName(model.path)
        property string filePath: model.path
        property string fileSize: i18n.tr("%1 file", "%1 files", model.count).arg(model.count)
        property bool isReadable: true
        property bool isExecutable: true
        property bool isWritable: true

        property bool isCurAllowedPath: true

        property var accessedDate
        property var modifiedDate
        property var creationDate

        property string mimeTypeDescription: i18n.tr("Folder")
        property string iconName: "folder"

        onOnlyAllowedPathsChanged: __checkIfIsWritable()
        onPathChanged: __checkIfIsWritable()

        Component.onCompleted: {
            // Add default allowed paths: XDG folders
            addAllowedDirectory(places.locationDocuments)
            addAllowedDirectory(places.locationDownloads)
            addAllowedDirectory(places.locationMusic)
            addAllowedDirectory(places.locationPictures)
            addAllowedDirectory(places.locationVideos)

            // Add default allowed paths: user's external media (i.e. "/media/USER/**")
            addAllowedDirectory(places.userMountLocation)
        }

        function __checkIfIsWritable() {
            if (model.path) {
                model.isWritable = model.curPathIsWritable() && (!model.onlyAllowedPaths || model.isAllowedPath(path))
                model.isCurAllowedPath = !model.onlyAllowedPaths || model.isAllowedPath(path)
                model.accessedDate = model.curPathAccessedDate()
                model.modifiedDate = model.curPathModifiedDate()
                model.creationDate = model.curPathCreatedDate()
            }
        }
    }

    property QtObject __places: PlacesModel {
        id: __places
    }

    property QtObject __archives: Archives {
        id: __archives
    }
}
