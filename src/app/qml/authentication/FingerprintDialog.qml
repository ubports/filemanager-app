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
import Biometryd 0.0 as BiometrydPlugin

Dialog {
    id: fingerprintDialog

    title: i18n.tr("Authentication required")
    text: i18n.tr("Use your fingerprint to access restricted content")

    signal succeeded()
    signal failed(var reason)
    signal aborted()
    signal usePasswordRequested()

    Row {
        spacing: units.gu(1)
        height: units.gu(8)

        Rectangle {
            id: fingerprintCircle
            width: units.gu(8); height: width; radius: width * 0.5
            color: UbuntuColors.blue

            Icon {
                anchors.fill: parent
                source: Qt.resolvedUrl("assets/fingerprint.png")
                color: "white"
            }
        }

        Label {
            id: fingerprintHint
            anchors.verticalCenter: parent.verticalCenter
            color: theme.palette.normal.backgroundTertiaryText
            // TRANSLATORS: "Touch" here is a verb
            text: i18n.tr("Touch sensor")
        }
    }

    Button {
        text: i18n.tr("Use password")
        color: UbuntuColors.graphite
        onClicked: {
            fingerprintDialog.usePasswordRequested()
            PopupUtils.close(fingerprintDialog)
        }
    }

    Button {
        text: i18n.tr("Cancel")
        color: UbuntuColors.graphite
        onClicked: {
            fingerprintDialog.aborted()
            PopupUtils.close(fingerprintDialog)
        }
    }

    /*** BIOMETRYD ***/
    BiometrydPlugin.Observer {
        id: biometryd

        property var operation: null

        function cancelOperation() {
            if (operation) {
                operation.cancel();
                operation = null;
            }
        }

        function restartOperation() {
            cancelOperation();

            var identifier = BiometrydPlugin.Biometryd.defaultDevice.identifier;
            operation = identifier.identifyUser();
            operation.start(biometryd);
        }

        Component.onCompleted: restartOperation()
        Component.onDestruction: cancelOperation()

        onSucceeded: {
            console.log("Identified user by fingerprint:", result);
            fingerprintDialog.succeeded()
            PopupUtils.close(fingerprintDialog)
        }

        onFailed: {
            console.log("Failed to identify user by fingerprint:", reason);
            authFailedAnimation.restart()
            fingerprintDialog.failed(reason)
        }
    }

    /*** ANIMATIONS ***/
    SequentialAnimation {
        id: authFailedAnimation

        ParallelAnimation {
            ColorAnimation {
                target: fingerprintCircle
                property: "color"
                from: UbuntuColors.blue
                to: UbuntuColors.red
                duration: 250
            }
            ScriptAction { script: fingerprintHint.text = i18n.tr("Authentication failed!") }
        }

        ParallelAnimation {
            ColorAnimation {
                target: fingerprintCircle
                property: "color"
                from: UbuntuColors.red
                to: UbuntuColors.blue
                duration: 250
            }
            ScriptAction { script: fingerprintHint.text = i18n.tr("Please retry") }
        }
    }
}

