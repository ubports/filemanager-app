import QtQuick 2.0
import Ubuntu.Components 0.1
import Ubuntu.Components.ListItems 0.1 as ListItem
import org.nemomobile.folderlistmodel 1.0

ListItem.Subtitled {
    text: model.fileName
    subText: Qt.formatDateTime(model.modifiedDate, Qt.DefaultLocaleShortDate) + (!model.isDir ? " " + fileSize : "")
    // FIXME: hard coded path for icon, assumes Ubuntu desktop icon available.
    // Nemo mobile has icon provider. Have to figure out what's the proper way
    // to get "system wide" icons in Ubuntu Touch, or if we have to use
    // icons packaged into the application. Both folder and individual
    // files will need an icon.
    icon: model.isDir ? "/usr/share/icons/gnome/22x22/apps/file-manager.png" : ""
    progression: model.isDir
}
