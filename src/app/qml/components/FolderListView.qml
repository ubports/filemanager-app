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
import Ubuntu.Components.ListItems 1.3 as OldListItems
import Ubuntu.Components.Popups 1.3
import org.nemomobile.folderlistmodel 1.0

Item {
    property FolderListModel folderListModel
    property string folderPath: folderListModel.path
    property bool smallMode: !wideAspect
    property Flickable flickable: root
    ListView {
        id: root
        anchors.fill: parent
        model: folderListModel
        // This must be visible so Autopilot can see it
        header: OldListItems.Header {
            objectName: "listViewSmallHeader"
            text: i18n.tr("%1 (%2 file)", "%1 (%2 files)", root.count).arg(folderPath).arg(root.count)
            height: smallMode ? units.gu(4) : 0
            clip: true
        }

        delegate: FolderListDelegate {
            id: delegate

            onClicked: itemClicked(model)

            onPressAndHold: itemLongPress(delegate, model)
        }
    }
    Scrollbar {
        flickableItem: root
        align: Qt.AlignTrailing
    }
}
