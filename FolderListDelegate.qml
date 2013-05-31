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
import Ubuntu.Components.ListItems 0.1 as ListItem
import org.nemomobile.folderlistmodel 1.0

ListItem.Subtitled {
    objectName: "folder" + index
    text: model.fileName
    subText: Qt.formatDateTime(model.modifiedDate, Qt.DefaultLocaleShortDate) + (!model.isDir ? " " + fileSize : "")
    // FIXME: hard coded path for icon, assumes Ubuntu desktop icon available.
    // Nemo mobile has icon provider. Have to figure out what's the proper way
    // to get "system wide" icons in Ubuntu Touch, or if we have to use
    // icons packaged into the application. Both folder and individual
    // files will need an icon.
    icon: model.isDir ? "/usr/share/icons/gnome/22x22/apps/file-manager.png" : ""
    progression: model.isDir
}
