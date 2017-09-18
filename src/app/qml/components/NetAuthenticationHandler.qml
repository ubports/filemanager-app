import QtQuick 2.4
import Ubuntu.Components 1.3
import "../dialogs"

Item  {
   id: netAuthenticatinHandler
   objectName: "netAuthenticatinHandler"

   property bool savePassword: true
   property var dialogObject: null

   function showDialog(urlPath,user) {
       if (dialogObject)
           return

       dialogObject = PopupUtils.open(netAuthenticationDialogComponent)
       dialogObject.showDialog(urlPath,user)
   }

   Timer {
       id: authTimer
       interval: 200
       repeat: false
       onTriggered:  {           
           pageModel.setPathWithAuthentication(
                         netAuthenticationDialog.currentPath,
                         netAuthenticationDialog.currentUserName,
                         netAuthenticationDialog.currentPassword,
                         netAuthenticatinHandler.savePassword
                        )
       }
   }

   Component {
        id: netAuthenticationDialogComponent

        NetAuthenticationDialog {
           id: netAuthenticationDialog
           onSavePasswordChanged: {
               savePassword = check
               console.log("NetAuthenticationHandler savePassword="+savePassword)
           }
           onOk: {
               if (!authTimer.running) {
                   authTimer.start()
               }
           }
           Component.onDestruction: {
               netAuthenticatinHandler.dialogObject = null
           }
       }
   }
}
