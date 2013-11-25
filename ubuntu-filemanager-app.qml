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
import Ubuntu.Components.Popups 0.1
import U1db 1.0 as U1db

/*!
    \brief MainView with Tabs element.
           First Tab has a single Label and
           second Tab has a single ToolbarAction.
*/

MainView {
    id: root
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "filemanager"
    applicationName: "com.ubuntu.filemanager"

    width: units.gu(100)
    height: units.gu(75)

    property alias filemanager: root

    property bool wideAspect: width >= units.gu(80)

    headerColor: "#464646"
    backgroundColor: "#797979"
    footerColor: "#808080"

//    headerColor: "#303030"
//    backgroundColor: "#505050"
//    footerColor: "#707070"

    property var pageStack: pageStack

    PageStack {
        id: pageStack

        Tabs {
            id: tabs

            Tab {
                title: page.title
                page: FolderListPage {
                    id: folderPage
                    objectName: "folderPage"

                    folder: homeFolder
                }
            }

//            Tab {
//                title: page.title
//                page: SettingsPage {
//                    id: settingsPage
//                }
//            }
        }

        Component.onCompleted: {
            pageStack.push(tabs)
        }
    }

    property var showToolbar: wideAspect ? true : undefined

    states: [
        State {
            when: showToolbar && toolbar.tools.opened && toolbar.tools.locked

            PropertyChanges {
                target: pageStack
                anchors.bottomMargin: -root.toolbar.triggerSize
            }
        }
    ]

    /* Settings Storage */

    U1db.Database {
        id: storage
        path: "ubuntu-filemanager-app.db"
    }

    U1db.Document {
        id: settings

        database: storage
        docId: 'settings'
        create: true

        defaults: {
            showAdvancedFeatures: false
        }
    }

    // Individual settings, used for bindings
    property bool showAdvancedFeatures: false

    property var viewMethod

    function getSetting(name, def) {
        var tempContents = {};
        tempContents = settings.contents
        var value = tempContents.hasOwnProperty(name)
                ? tempContents[name]
                : settings.defaults.hasOwnProperty(name)
                  ? settings.defaults[name]
                  : def
        //print(name, JSON.stringify(def), JSON.stringify(value))
        return value
    }

    function saveSetting(name, value) {
        if (getSetting(name) !== value) {
            //print(name, "=>", value)
            var tempContents = {}
            tempContents = settings.contents
            tempContents[name] = value
            settings.contents = tempContents

            reloadSettings()
        }
    }

    function reloadSettings() {
        //showAdvancedFeatures = getSetting("showAdvancedFeatures", false)
        viewMethod = getSetting("viewMethod", width > units.gu(40) ? i18n.tr("Icons") : i18n.tr("List"))
    }

    Component.onCompleted: {
        reloadSettings()
    }

    function getIcon(name) {
        return /*"/usr/share/icons/ubuntu-mobile/actions/scalable/" + name + ".svg" */Qt.resolvedUrl("icons/" + name + ".png")
    }

    Component {
        id: folderListPageComponent

        Tab {
            property alias folder: pageFolder.folder
            title: page.title
            page: FolderListPage {
                id: pageFolder
            }
        }
    }

    function openFile(filePath) {
        if (!folderPage.model.openPath(filePath)) {
            error(i18n.tr("File operation error"), i18n.tr("Unable to open '%11").arg(filePath))
        }
    }

    function error(title, message) {
        PopupUtils.open(Qt.resolvedUrl("NotifyDialog.qml"), root,
                        {
                            title: title,
                            text: message
                        })
    }

    Keys.onPressed: {
        print("Key pressed!")
        event.accepted = tabs.currentPage.keyPressed(event.key, event.modifiers)
    }
}
