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
import Ubuntu.Components.Popups 1.3
import org.nemomobile.folderlistmodel 1.0

import "../actions" as FMActions
import "../components"

ScrollView {
    id: folderListView

    property var folderListPage
    property var fileOperationDialog
    property FolderListModel folderListModel
    property string folderPath: folderListModel.path

    ListView {
        id: root
        anchors.fill: parent
        model: folderListModel
        // This must be visible so Autopilot can see it
        header: SectionDivider {
            objectName: "listViewSmallHeader"
            text: i18n.tr("%1 (%2 file)", "%1 (%2 files)", root.count).arg(folderPath).arg(root.count)
        }

        delegate: FolderListDelegate {
            id: delegate

            property var __delegateActions: FolderDelegateActions {
                folderListPage: folderListView.folderListPage
                folderListModel: folderListView.folderListModel
                fileOperationDialog: folderListView.fileOperationDialog
            }

            leadingActions: ListItemActions {
                // Children is an alias for 'actions' property, this way we don't get any warning about non-NOTIFYable props
                actions: __delegateActions.leadingActions.children
            }

            trailingActions: ListItemActions {
                // Children is an alias for 'actions' property, this way we don't get any warning about non-NOTIFYable props
                actions: __delegateActions.trailingActions.children
            }

            onClicked: __delegateActions.itemClicked(model)
            onPressAndHold: __delegateActions.listLongPress()
        }
    }
}
