import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

Dialog {
    id: root

    property alias inputText: input.text
    signal accepted
    signal rejected

    TextField {
        id: input
        focus: true
    }

    Button {
        text: i18n.tr("Ok")
        onClicked: {
            accepted()
            PopupUtils.close(root)
        }
    }

    Button {
        text: i18n.tr("Cancel")
        onClicked: {
            rejected()
            PopupUtils.close(root)
        }
    }
}
