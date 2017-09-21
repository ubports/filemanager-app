import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Page {
    id: imagePreview
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
                    PopupUtils.open(Qt.resolvedUrl("../ui/FileDetailsPopover.qml"), imagePreview, props)
                }
            }
        ]
    }

    Flickable {
        id: flick
        anchors {
            top: header.bottom
            fill: parent
            horizontalCenter: parent.horizontalCenter
            verticalCenter: parent.verticalCenter
        }
        contentWidth: parent.width
        contentHeight: parent.height

        PinchArea {
            width: Math.max(flick.contentWidth, flick.width)
            height: Math.max(flick.contentHeight, flick.height)

            property real initialWidth
            property real initialHeight
            onPinchStarted: {
                initialWidth = flick.contentWidth
                initialHeight = flick.contentHeight
            }

            onPinchUpdated: {
                // adjust content pos due to drag
                flick.contentX += pinch.previousCenter.x - pinch.center.x
                flick.contentY += pinch.previousCenter.y - pinch.center.y

                // resize content
                flick.resizeContent(initialWidth * pinch.scale, initialHeight * pinch.scale, pinch.center)
            }

            onPinchFinished: {
                // Move its content within bounds.
                flick.returnToBounds()
            }

            Rectangle {
                width: flick.contentWidth
                height: flick.contentHeight
                color: "white"
                AnimatedImage {
                    anchors.fill: parent
                    source: model.filePath
                    fillMode: Image.PreserveAspectFit
                    MouseArea {
                        anchors.fill: parent
                        onDoubleClicked: {
                            flick.contentWidth = imagePreview.width
                            flick.contentHeight = imagePreview.height
                        }
                    }
                }
            }
        }
    }
}
