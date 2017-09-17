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
import QtQuick 2.4
import Ubuntu.Components 1.3
import org.nemomobile.folderlistmodel 1.0

ListItem {
    objectName: "folder" + index

    property string fileName: model.fileName
    property string filePath: model.filePath
    height: layout.height
    // Because Flickable is used over ListView??
    // we cannot set the highlight component so
    // instead fudge it here with a rectangle.
    Rectangle {
        anchors.fill: parent
        color: UbuntuColors.silk
        visible: model.isSelected
    }

    ListItemLayout {
        id: layout
        title.text: model.fileName
        subtitle.text: itemDateAndSize(model)

        Icon {
            property string path: fileView.folder + '/' + model.fileName
            source: fileIcon(path, model)
            height: units.gu(5); width: height
            SlotsLayout.position: SlotsLayout.Leading
        }

        ProgressionSlot{
            visible: model.isBrowsable
        }
 }
}
