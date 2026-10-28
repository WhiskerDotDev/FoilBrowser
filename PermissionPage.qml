import QtQuick;
import QComponents;
import QtWebEngine;

Column{
      id: root;
      required property webEnginePermission requestedPermission;

      signal requestClose;
      property point btnSize: Qt.point(40, 20);

      function getDescription() : string{
            let desc;
                  switch(root.requestedPermission.permissionType){
                  case WebEnginePermission.PermissionType.MediaAudioCapture: desc = "Allow website to record audio?"; break;
                  case WebEnginePermission.PermissionType.MediaVideoCapture: desc = "Allow website to record video?"; break;
                  case WebEnginePermission.PermissionType.MediaAudioVideoCapture: desc = "Allow website to record audio and video?"; break;
                  case WebEnginePermission.PermissionType.DesktopVideoCapture: desc = "Allow website to capture screen contents?"; break;
                  case WebEnginePermission.PermissionType.DesktopAudioVideoCapture: desc = "Allow website to capture screen contents and audio?"; break;
                  case WebEnginePermission.PermissionType.Geolocation: desc = "Allow website to access your location?"; break;
                  case WebEnginePermission.PermissionType.ClipboardReadWrite: desc = "Allow website to access and modify clipboard contents?"; break;
                  case WebEnginePermission.PermissionType.LocalFontsAccess: desc = "Allow website to access fonts stored on this computer?"; break;
                  case WebEnginePermission.PermissionType.MouseLock: desc = "Allow website to use Pointer Lock?"; break;
                  // Notifications will be automatically denied up the chain.
                  }
            return desc;
            }

      StyledTextEdit{
            id: content;
            width: parent.width;
            height: parent.height - root.btnSize.y;
            wrapMode: Text.Wrap;
            smooth: false; antialiasing: false;
            fontPixelSize: 14
            text: root.getDescription() + "\nClosing the dialog denies the permission.";
            cursorVisible: true;
            cursorPosition: text.length; readOnly: true;
            color: "#505050";
      }

      Row{
            height: root.btnSize.y;
            spacing: 2;
            x: ( (parent.width / 2) - root.btnSize.x ) + spacing/2;
            y: (parent.height - root.btnSize.y);
            TextButton{
                  id: acceptButton;
                  size:root.btnSize;
                  hitbox.onSingleTapped: ()=>{root.requestedPermission.grant(); root.requestClose(); }
                  text: "accept";
            }
            TextButton{
                  id:rejectButton;
                  size:root.btnSize;
                  hitbox.onSingleTapped: ()=>{root.requestedPermission.deny(); root.requestClose(); }
                  text: "reject";
            }
      }

      // We don't need to handle the parent window closing. GC will take care of the permission request.
      // Hence, we can just leave the permission unanswered.

}