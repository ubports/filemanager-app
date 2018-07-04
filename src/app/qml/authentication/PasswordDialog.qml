/*
 * Copyright (C) 2017 Stefano Verzegnassi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License 3 as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see http://www.gnu.org/licenses/.
 */

import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog {
    id: authenticationDialog

    property bool isAlphanumeric: true
    property var pamAuthentication

    function authenticate() {
        if (pamAuthentication.validatePasswordToken(passwordField.text)) {
            authenticationDialog.succeeded()
            PopupUtils.close(authenticationDialog)
        } else {
            errorHint.visible = true
            passwordField.text = ""
            authenticationDialog.failed(i18n.tr("Authentication failed"))
        }
    }

    signal succeeded()
    signal failed(var reason)
    signal aborted()

    title: i18n.tr("Authentication required")
    text: authenticationDialog.isAlphanumeric
          ? i18n.tr("Your passphrase is required to access restricted content")
          : i18n.tr("Your passcode is required to access restricted content")

    Component.onCompleted: {
        if (!authenticationDialog.isAlphanumeric)
            passwordField.inputMethodHints |= Qt.ImhDigitsOnly

        passwordField.forceActiveFocus()
    }

    TextField {
        id: passwordField
        placeholderText: handler.isPassphrase ? i18n.tr("passphrase (default is 0000 if unset)") : i18n.tr("passcode (default is 0000 if unset)")
        echoMode: TextInput.Password

        inputMethodHints: Qt.ImhHiddenText | Qt.ImhSensitiveData |
                          Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText

        onAccepted: authenticationDialog.authenticate()
    }

    Label {
        id: errorHint
        color: UbuntuColors.red
        text: i18n.tr("Authentication failed. Please retry")
        visible: false
    }

    Button {
        text: i18n.tr("Authenticate")
        color: UbuntuColors.green
        onClicked: authenticationDialog.authenticate()
    }

    Button {
        text: i18n.tr("Cancel")
        color: UbuntuColors.graphite
        onClicked: {
            authenticationDialog.aborted()
            PopupUtils.close(authenticationDialog)
        }
    }
}
