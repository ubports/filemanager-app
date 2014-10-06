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
import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.ListItems 1.0 as ListItem
import org.nemomobile.folderlistmodel 1.0

ListItem.Subtitled {
    objectName: "folder" + index

    property string fileName: model.fileName
    property string filePath: path

    text: model.fileName
    subText: Qt.formatDateTime(model.modifiedDate, Qt.DefaultLocaleShortDate) + (!model.isDir ? ", " + fileSize : "")

    property string path: fileView.folder + '/' + model.fileName
    iconSource: fileIcon(path, model.isDir)

    progression: model.isDir
    iconFrame: false

    selected: model.isSelected
}
