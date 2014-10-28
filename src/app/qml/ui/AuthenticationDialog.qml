/*
 * Copyright (C) 2014 Canonical Ltd
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

Dialog {
    id: root

    title: i18n.tr("Authentication required")

    text: i18n.tr("Password is required to access all files")

    signal passwordEntered(string password)

    Component.onCompleted: {
        passwordField.forceActiveFocus()
    }

    TextField {
        id: passwordField
        objectName: "inputField"

        placeholderText: i18n.tr("password")
        echoMode: TextInput.Password

        onAccepted: okButton.clicked(text)
    }

    Button {
        id: okButton
        objectName: "okButton"

        text: i18n.tr("Ok")

        onClicked: {
            passwordEntered(passwordField.text)
            PopupUtils.close(root)
        }
    }

    Button {
        id: cancelButton
        objectName: "cancelButton"
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
            PopupUtils.close(root)
        }
    }
}
