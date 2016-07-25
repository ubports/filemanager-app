/*
 * Copyright (C) 2014 Canonical Ltd
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

import Ubuntu.Content 1.3

import "contenttyperesolver.js" as Resolver

Page {
    id: root
    title: i18n.tr("Open with")

    property var activeTransfer

    property string fileUrl
    property bool share: false

    Component.onCompleted: {
        var contentType = Resolver.resolveContentType(fileUrl)
        console.log("Resolved contenttype: " + contentType)
        peerPicker.contentType = contentType
    }

    Component {
        id: resultComponent
        ContentItem {}
    }

    function __exportItemsWhenPossible(url) {
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
        handler: root.share ? ContentHandler.Share : ContentHandler.Destination
        contentType: ContentType.Pictures

        onPeerSelected: {
            root.activeTransfer = peer.request();
            pageStack.pop();
            __exportItemsWhenPossible(root.fileUrl)
        }

        onCancelPressed: {
            pageStack.pop();
        }
    }

    Connections {
        target: root.activeTransfer
        onStateChanged: {
            console.log("curTransfer StateChanged: " + root.activeTransfer.state);
            __exportItemsWhenPossible(root.fileUrl)
        }
    }
}
