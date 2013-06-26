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
import org.nemomobile.folderlistmodel 1.0

Dialog {
    id: root
    property FolderListModel folderListModel
    property string descriptionPrepend: i18n.tr("Operation in progress")

    title: "File operation"
    text: descriptionPrepend

    ProgressBar {
        id: progress
        minimumValue: 0.0
        maximumValue: 100.0
        value: 0.0
    }

    Button {
        text: i18n.tr("Cancel")
        onClicked: {
            console.log("Cancelling file progress action")
            folderListModel.cancelAction()
            PopupUtils.close(root)
        }
    }

    Connections {
        target: folderListModel
        onProgress: {
            console.log("On progress ", curItem, totalItems, percent)
            progress.value = percent
            if (curItem == totalItems) {
                console.log("All files processed, closing progress dialog")
                PopupUtils.close(root)
            } else {
                root.text = descriptionPrepend + " " + curItem + "/" + totalItems
            }
        }
    }

    // Errors from model
    Connections {
        target: pageModel
        onError: {
            PopupUtils.close(root)
        }
    }
}
