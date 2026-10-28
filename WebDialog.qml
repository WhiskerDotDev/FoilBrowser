import QtQuick;
import QtWebEngine
import QComponents;

Window{
      id: root;
      flags: Qt.FramelessWindowHint  | Qt.SubWindow;
      width: 400; height: 300;

      ResizeEdges{ win: root; horizontalBounds: 3; verticalBounds: 2;}
      TitleBar{
            id: titleBar;
            win: root;
            height: 15;
            DefaultWindowControls{
                  win: parent.win;
                  btnsize: Qt.point(20, parent.height -5 );
            }
      }

      property WebEngineView view: WebEngineView{
            parent: root.contentItem;
            height: parent.height - titleBar.height;
            width: parent.width;
            y: titleBar.height;
            url: "about:blank";
      }

      // TODO: Update to a property-based system;
      // try implementing a focus-based sleep timer?

}
