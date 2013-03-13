import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.Popups 0.1

Popover {
    id: root
    property var model

    function dateTimeFormat(dateTime) {
        return Qt.formatDateTime(dateTime, Qt.DefaultLocaleShortDate)
    }

    function permissionsToString(model) {
        var permissions = []
        if (model.isReadable) {
            permissions.push(i18n.tr("Readable"))
        }
        if (model.isWritable) {
            permissions.push(i18n.tr("Writable"))
        }
        if (model.isExecutable) {
            permissions.push(i18n.tr("Executable"))
        }

        // Now why does this not work?
        // return permissions.join(", ")
        var permStr = ""
        for (var i = 0; i < permissions.length; ++i) {
            if (permStr.length > 0) {
                permStr += ", "
            }
            permStr += permissions[i]
        }
        return permStr
    }

    Column {
        anchors.margins: units.gu(2)
        Row {
            Image {
                // TODO: how to get proper icon?
                source: model.isDir
                        ? "/usr/share/icons/gnome/48x48/apps/file-manager.png"
                        : "/usr/share/icons/gnome/48x48/mimetypes/text-x-preview.png"
            }
            Label {
                text: model.fileName
                anchors.verticalCenter: parent.verticalCenter
            }
        }
        Grid {
            columns: 2
            spacing: units.gu(1)
            Label {
                text: i18n.tr("Size:")
            }
            Label {
                text: model.fileSize
            }

            Label {
                text: i18n.tr("Created:")
            }
            Label {
                text: dateTimeFormat(model.creationDate)
            }

            Label {
                text: i18n.tr("Modified:")
            }
            Label {
                text: dateTimeFormat(model.modifiedDate)
            }

            Label {
                text: i18n.tr("Permissions:")
            }
            Label {
                text: permissionsToString(model)
            }

        }
    }
}
