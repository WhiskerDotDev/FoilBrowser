pragma ComponentBehavior: Bound;
import QtQuick;
import QtWebEngine;
import QComponents;

Column{
      id: root;
      property point btnSize: Qt.point(40, 20);
      signal requestClose;
      property WebEngineDownloadRequest _req: null; //consider making the WebEngineView it is from to go back.
      Connections{
      target: root._req;
            function onStateChanged(){
            if(root._req.state !== WebEngineDownloadRequest.DownloadState.DownloadRequested){root.requestClose();} }
      }

      Loader{
            id: ldr;
            active: true/*root.parent instanceof PageLoaderPopup*/;
            sourceComponent: Component{
                  Connections{
                        target: root.parent;
                        function onClosing(close){
                              close.accepted = false;
                              if(root._req.state !== WebEngineDownloadRequest.DownloadRequested)root._req.cancel(); // cancel if the download request hasn't been handled from beforehand
                              close.accepted = true;
                        }
                  }
            }
      }

      StyledTextEdit{
            id: content;
            width: parent.width;
            height: parent.height - root.btnSize.y;
            wrapMode: Text.Wrap;
            // smooth: false; antialiasing: false;
            fontPixelSize: 14
            text: "-file: " + root._req.suggestedFileName + "\n-path: " + root._req.downloadDirectory;
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
                  hitbox.onSingleTapped: ()=>{root._req.accept();}
                  text: "accept";
            }
            TextButton{
                  id:rejectButton;
                  size:root.btnSize;
                  hitbox.onSingleTapped: ()=>{root._req.cancel();}
                  text: "reject";
            }
      }

Keys.onReturnPressed: ()=>{root._req.accept();}
Keys.onEscapePressed:()=>{root._req.cancel();}
// no need to handle keys.onEscape pressed
// or maybe, we do...

}
