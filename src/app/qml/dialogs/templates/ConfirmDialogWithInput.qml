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

Dialog {
    id: root

    property alias inputText: input.text
    property alias placeholderText: input.placeholderText
    signal accepted(var text)
    signal rejected

    Component.onCompleted: {
        // Need to force active focus to input, otherwise the parent object that created this
        // dialog will continue to have active focus.
        input.forceActiveFocus()
    }

    TextField {
        id: input
        objectName: "inputField"
        focus: true
        // Avoid need to press enter to make "Ok" button enabled.
        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
        validator: RegExpValidator {
            regExp: /.+/
        }
    }

    Button {
        objectName: "okButton"
        text: i18n.tr("OK")
        enabled: input.acceptableInput
        color: UbuntuColors.green
        onClicked: {
            accepted(input.text)
            PopupUtils.close(root)
        }
    }

    Button {
        objectName: "cancelButton"
        text: i18n.tr("Cancel")
        color: UbuntuColors.graphite

        onClicked: {
            rejected()
            PopupUtils.close(root)
        }
    }
}
