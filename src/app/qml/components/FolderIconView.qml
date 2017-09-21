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
 * Authored by: Michael Spencer <sonrisesoftware@gmail.com>
 */
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

ScrollView {
    id: folderIconView

    property var folderListPage
    property var fileOperationDialog
    property var folderModel

    property alias footer: view.footer
    property alias header: view.header

    function calcCellwidth () {
        return folderListPage.width / ((folderListPage.width / units.gu(12)).toFixed(0))
    }

    GridView {
        id: view
        anchors.fill: parent

        cellWidth: calcCellwidth()
        cellHeight: units.gu(14)

        model: folderModel.model
        delegate: FolderIconDelegate {
            id: delegate
            width: units.gu(12)
            height: view.cellHeight

            iconName: model.iconName
            title: model.fileName
            isSelected: model.isSelected
            path: model.filePath

            property var __delegateActions: FolderDelegateActions {
                folderListPage: folderIconView.folderListPage
                folderModel: folderIconView.folderModel
                fileOperationDialog: folderIconView.fileOperationDialog
            }

            onClicked: {
                if (mouse.button === Qt.RightButton) {
                    delegate.pressAndHold(mouse)
                } else {
                    __delegateActions.itemClicked(model)
                }
            }

            onPressAndHold: __delegateActions.itemLongPress(delegate, model)
        }
    }
}

