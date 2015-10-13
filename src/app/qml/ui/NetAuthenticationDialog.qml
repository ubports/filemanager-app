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
import QtQuick 2.3
import Ubuntu.Components 1.1
import Ubuntu.Components.Popups 1.0
import Ubuntu.Components.ListItems 1.0

Dialog {
    id: authenticationDialog
    objectName: "authenticationDialog"
    title: i18n.tr("Authentication required")

    property alias  currentPath:     authCurrentPath.text
    property alias  currentUserName: authUserName.text
    property alias  currentPassword: authPassword.text   

    signal ok()
    signal savePasswordChanged(bool check)

    function showDialog(path,user) {
        currentPath     = path
        currentUserName = user
        autcheckSavePassword.checked    = true
        authenticationDialog.show()
    }

    Component.onCompleted: {
        authUserName.forceActiveFocus()
        authUserName.cursorPosition = authUserName.text.length
    }

    Text {
        id: authCurrentPath
        anchors.horizontalCenter: parent.horizontalCenter
        font.italic: true
        elide: Text.ElideMiddle
    }

    Text {
        text: i18n.tr("User")
    }

    TextField {
        id: authUserName
        objectName: "authUserName"
        visible: true
        focus: true
    }

    Text {
        text: i18n.tr("Password")
    }

    TextField {
        id: authPassword
        objectName: "authPassword"
        echoMode: TextInput.Password
        focus: true
        onAccepted: authOkButton.clicked()
    }

    Standard {
        Label {
            text: i18n.tr("Save password")
            color: Theme.palette.normal.overlayText
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
        }
        control: CheckBox {
            id: autcheckSavePassword
            objectName: "autcheckSavePassword"
            anchors.verticalCenter: parent.verticalCenter
            onCheckedChanged: {
                console.log("NetAuthenticationDialog::onCheckedChanged() checked="+checked)
                savePasswordChanged(checked)
            }
        }
    }

    Button {
        id: authOkButton
        objectName: "authOkButton"
        text: i18n.tr("Ok")
        onClicked: {
            ok()
            PopupUtils.close(authenticationDialog)
        }
    }

    Button {
        id: authCancelButton
        objectName: "authCancelButton"
        text: i18n.tr("Cancel")
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
        onClicked: {
            PopupUtils.close(authenticationDialog)
        }
    }//authCancelButton
}
