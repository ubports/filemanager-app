/*
 * Copyright (C) 2015 Canonical Ltd
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
 * Authored by: Carlos Jose Mazieri <carlos.mazieri@gmail.com>
 */
import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import QtQuick.Layouts 1.1

import "../components"

Dialog {
    id: authenticationDialog
    objectName: "authenticationDialog"
    title: i18n.tr("Authentication required")

    property alias  currentPath:     authenticationDialog.text
    property alias  currentUserName: authUserName.text
    property alias  currentPassword: authPassword.text
    property alias  savePassword: autcheckSavePassword.checked

    signal ok()

    Component.onCompleted: {
        authUserName.forceActiveFocus()
        authUserName.cursorPosition = authUserName.text.length
    }

    Label {
        color: theme.palette.normal.backgroundTertiaryText
        textSize: Label.Small
        text: i18n.tr("User")
    }

    TextField {
        id: authUserName
        objectName: "authUserName"
        visible: true
        focus: true
    }

    Label {
        color: theme.palette.normal.backgroundTertiaryText
        textSize: Label.Small
        text: i18n.tr("Password")
    }

    TextField {
        id: authPassword
        objectName: "authPassword"
        echoMode: TextInput.Password
        focus: true
        onAccepted: authOkButton.clicked()
    }

    ListItem {
        divider.visible: false
        ListItemLayout {
            title.text: i18n.tr("Save password")
            title.color: theme.palette.normal.backgroundSecondaryText

            Switch {
                SlotsLayout.position: SlotsLayout.Last
                id: autcheckSavePassword
                objectName: "autcheckSavePassword"
                onCheckedChanged: {
                    console.log("NetAuthenticationDialog::onCheckedChanged() checked="+checked)
                    savePasswordChanged(checked)
                }

            }
        }
    }

    RowLayout {
        anchors { left: parent.left; right: parent.right }
        height: units.gu(4)
        layoutDirection: Qt.RightToLeft

        Button {
            id: authCancelButton
            objectName: "authCancelButton"
            text: i18n.tr("Cancel")
            Layout.fillHeight: true
            Layout.fillWidth: true

            onClicked: {
                PopupUtils.close(authenticationDialog)
            }
        }

        Button {
            id: authOkButton
            objectName: "authOkButton"
            text: i18n.tr("Ok")
            Layout.fillHeight: true
            Layout.fillWidth: true

            onClicked: {
                ok()
                PopupUtils.close(authenticationDialog)
            }
        }
    }
}
