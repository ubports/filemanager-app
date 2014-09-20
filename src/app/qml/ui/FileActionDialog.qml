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
import Ubuntu.Components.Popups 1.0
import org.nemomobile.folderlistmodel 1.0

Dialog {
    id: root

    property string fileName
    property string filePath
    property FolderListModel folderListModel

    title: i18n.tr("Choose action")
    text: i18n.tr("For file: %1").arg(fileName)

    Button {
        objectName: "openButton"
        text: i18n.tr("Open")
        onClicked: {
            console.log("Opening file", filePath)
            openFile(model.filePath)
            onClicked: PopupUtils.close(root)
        }
    }

    Button {
        objectName: "cancelButton"
        text: i18n.tr("Cancel")
        onClicked: PopupUtils.close(root)

        gradient: Gradient {
            GradientStop {
                position: 0
                color: "gray"
            }

            GradientStop {
                position: 1
                color: "lightgray"
            }
        }
    }
}
