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
import org.nemomobile.folderlistmodel 1.0

/*!
    \brief MainView with Tabs element.
           First Tab has a single Label and
           second Tab has a single ToolbarAction.
*/

MainView {
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "filemanager"
    applicationName: "ubuntu-filemanager-app"
    
    width: units.gu(50)
    height: units.gu(75)
    
    PageStack {
        objectName: "pageStack"
        id: pageStack

        Component.onCompleted: goHome()

        // FIXME: This is only used to get the user's home folder!!!
        FolderListModel {
            id: tempModel
        }

        function goHome() {
            // FIXME: Get the user's home folder without requiring an instance
            // of a FolderListModel
            goTo(tempModel.homePath())
        }

        function goTo(folder) {
            pageStack.clear()

            var items = folder.split('/')

            var path = ""
            for (var i = 0; i < items.length; i++) {
                path = path + "/" + items[i]
                path = path.replace("//", "/")
                pageStack.push(Qt.resolvedUrl("FolderListPage.qml"), {"folder": path})
            }


        }
    }
}
