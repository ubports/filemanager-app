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
import Ubuntu.Components.ListItems 0.1
import Ubuntu.Components.Popups 0.1
import org.nemomobile.folderlistmodel 1.0

ListView {
    id: root

    property FolderListModel folderListModel
    property string folderPath: folderListModel.path
    model: folderListModel

    // This must be visible so Autopilot can see it
    header: Header {
        objectName: "listViewSmallHeader"
        text: (root.count == 1
               ? i18n.tr("%1 (1 file)").arg(root.folderPath)
               : i18n.tr("%1 (%2 files)").arg(root.folderPath).arg(root.count))
        height: smallMode ? units.gu(4) : 0
        clip: true
    }

    delegate: FolderListDelegate {
        id: delegate

        onClicked: itemClicked(model)

        onPressAndHold: itemLongPress(delegate, model)
    }

    Scrollbar {
        flickableItem: root
        align: Qt.AlignTrailing
    }
}
