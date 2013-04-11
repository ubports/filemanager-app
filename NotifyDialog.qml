import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

Dialog {
    id: root
    Button {
        text: i18n.tr("Ok")
        onClicked: {
            PopupUtils.close(root)
        }
    }
}
