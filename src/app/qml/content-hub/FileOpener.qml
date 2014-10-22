import QtQuick 2.0
import Ubuntu.Components 1.1

import Ubuntu.Content 0.1

Page {
    id: root
    title: i18n.tr("Open with")

    property var activeTransfer

    property string fileUrl

    Component {
        id: resultComponent
        ContentItem {}
    }

    function __exportItems(url) {
        if (root.activeTransfer.state === ContentTransfer.InProgress)
        {
            root.activeTransfer.items = [ resultComponent.createObject(root, {"url": url}) ];
            root.activeTransfer.state = ContentTransfer.Charged;
        }
    }
    ContentPeerPicker {
        id: peerPicker
        showTitle: false

        // Type of handler: Source, Destination, or Share
        handler: ContentHandler.Destination
        contentType: ContentType.Pictures

        onPeerSelected: {
            root.activeTransfer = peer.request();
            pageStack.pop();
            if (root.activeTransfer.state === ContentTransfer.InProgress) {
                root.__exportItems(root.fileUrl);
            }
        }

        onCancelPressed: {
            pageStack.pop();
        }
    }
}
