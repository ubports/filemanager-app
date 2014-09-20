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

Dialog {
    id: root

    property alias inputText: input.text
    property alias placeholderText: input.placeholderText
    signal accepted
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
        validator: RegExpValidator {
            regExp: /.+/
        }
    }

    Button {
        objectName: "okButton"
        text: i18n.tr("Ok")
        enabled: input.acceptableInput
        onClicked: {
            accepted()
            PopupUtils.close(root)
        }
    }

    Button {
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
            rejected()
            PopupUtils.close(root)
        }
    }
}
