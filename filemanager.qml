import QtQuick 2.0
import Ubuntu.Components 0.1
import org.nemomobile.folderlistmodel 1.0

/*!
    \brief MainView with Tabs element.
           First Tab has a single Label and
           second Tab has a single ToolbarAction.
*/

MainView {
    // objectName for functional testing purposes (autopilot-qt5)
    objectName: "filemanager"
    
    width: units.gu(50)
    height: units.gu(75)
    
    Tabs {
        id: tabs
        anchors.fill: parent
        
        // First tab begins here
        Tab {
            objectName: "folderListPage"
            
            title: i18n.tr("File Manager")
            
            // Tab content begins here
            page: FolderListPage {
            }
        }
        
        // Second tab begins here
        Tab {
            objectName: "bookmarksPage"
            
            title: i18n.tr("Bookmarks")
            page: BookmarksPage {
            }
        }
    }
}
