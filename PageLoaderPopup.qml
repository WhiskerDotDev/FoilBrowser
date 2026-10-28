import QtQuick;
import QComponents;

Window{
      id: rootwin;
      required property Window win;
      contentItem{
            smooth: false; }
      visible: true;
      width: 200; height: 200; // 200 x 200 or similar. preferrably.
      flags: Qt.SubWindow  | Qt.FramelessWindowHint;
      color: win.color;
      property bool destroyOnClose: true;
      property bool closeOnEscape: false;
      Shortcut{
      sequence: "Esc";
      enabled: rootwin.closeOnEscape;
      context: Qt.WindowShortcut;
      onActivated: rootwin.close();
      }

TitleBar{
            id: m_titleBar;
            height: 15;
            win: rootwin;
            enableResize: false;
            z: 1;
            IconButton{
                  id:m_closeBtn;
                  color: "red";
                  visible: true;
                  height: parent.height - 3; // 12 px;
                  width: 20;
                  hitbox.onSingleTapped: ()=>{ rootwin.close(); }
                  // Remember to expose the icon.
                  anchors.right: parent.right;
                  anchors.rightMargin: 1;
                  anchors.verticalCenter: parent.verticalCenter;
            }
      }
      property alias closeButton: m_closeBtn;
      property alias titleBar: m_titleBar

      property Item content;

      Component.onCompleted:()=>{
                                  content.parent = rootwin.contentItem;
                                  content.anchors.fill = rootwin.contentItem;
                                  content.anchors.topMargin = m_titleBar.height;
                                  content.smooth = false; content.antialiasing = false;
                                  rootwin.requestActivate();
                                  content.forceActiveFocus();
                            }

      Connections{
            target: rootwin.content;
            ignoreUnknownSignals: true;
            function onRequestClose(){
                  // console.log("close requested!");
                  rootwin.close();
            }
      }
      onClosing:()=>{if(rootwin.destroyOnClose === true)destroy(); }
}
