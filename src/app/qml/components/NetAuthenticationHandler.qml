import QtQuick 2.4
import Ubuntu.Components 1.3
import "../ui"

Item  {
   id: netAuthenticatinHandler
   objectName: "netAuthenticatinHandler"

   property bool savePassword: true
   function showDialog(urlPath,user) {
       console.log("needsAuthenticationHandler::showDialog()")
       netAuthenticationDialog.showDialog(urlPath,user)
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
   }
}
