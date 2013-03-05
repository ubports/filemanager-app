import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

Popover {
    id: root
    property string filePath

    Column {
        Label {
            text: "\n\nTODO file details for " + filePath + "\n\n"
        }
    }
}
