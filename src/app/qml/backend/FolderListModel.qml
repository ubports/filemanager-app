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
import org.nemomobile.folderlistmodel 1.0

FolderListModel {
    id: pageModel
    enableExternalFSWatcher: true

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

    onOnlyAllowedPathsChanged: checkIfIsWritable()
    onPathChanged: checkIfIsWritable()
}
