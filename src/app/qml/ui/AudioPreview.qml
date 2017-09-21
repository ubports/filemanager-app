import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3
import QtMultimedia 5.6

Page {
    id: audioPreview
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
                    PopupUtils.open(Qt.resolvedUrl("../ui/FileDetailsPopover.qml"), audioPreview, props)
                }
            }
        ]
    }

    Text {
        anchors {
            top: header.bottom
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        text: i18n.tr("Play");
        font.pointSize: 24;
        width: 150; height: 50;

        MediaPlayer {
            id: mediaplayer
            source: model.filePath
        }
        MouseArea {
            id: playArea
            anchors.fill: parent
            onPressed:  { mediaplayer.play() }
        }
    }
}
