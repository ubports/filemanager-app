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

Dialog {
    id: root
    property FolderListModel model
    property Page page
    property string descriptionPrepend: i18n.tr("Operation in progress")

    title: i18n.tr("File operation")
    text: descriptionPrepend

    function startOperation(name) {
        root.title = name
    }

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
            model.cancelAction()
            PopupUtils.close(root)
        }
    }

    Connections {
        target: model
        onProgress: {
            // curItem == 0 && percent == 0 means the Action has just been created, check getProgressCounter() before
            if (curItem == 0 && percent == 0) {
                console.log("Creating dialog:", model.getProgressCounter())
                if (model.getProgressCounter()  > 20) {
                    // show/activate/make visible    the dialog here
                    print("Showing dialog...")
                    PopupUtils.open(root, page)
                    root.show()
                }
            }

            console.log("On progress ", curItem, totalItems, percent)

            progress.value = percent
            if (percent == 100 && curItem == totalItems) {
                console.log("All files processed, closing progress dialog")
                //PopupUtils.close(root)
                root.hide()
            } else {
                root.text = descriptionPrepend + " (" + curItem + "/" + totalItems + ")"
            }
        }
    }

    // Errors from model
    Connections {
        target: model
        onError: {
            PopupUtils.close(root)
        }
    }
}
