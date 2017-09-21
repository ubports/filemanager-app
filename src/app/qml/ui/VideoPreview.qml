import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import QtMultimedia 5.6

Page {
    id: videoPreview
    property var model

    header: PageHeader {
        title: model.fileName
        trailingActionBar.actions: [
            Action {
                iconName: "external-link"
                onTriggered: {
                    openLocalFile(model.filePath)
                }
            },
            Action {
                iconName: "info"
                onTriggered: {
                    var props = {
                        "model": model
                    }
                    PopupUtils.open(Qt.resolvedUrl("../ui/FileDetailsPopover.qml"), videoPreview, props)
                }
            }
        ]
    }

    Item {
        anchors {
            top: header.bottom
            fill: parent
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        MediaPlayer {
            id: mediaplayer
            source: model.filePath
        }

        VideoOutput {
            anchors.fill: parent
            source: mediaplayer
        }

        MouseArea {
            id: playArea
            anchors.fill: parent
            onPressed: mediaplayer.play();
        }
    }
}
