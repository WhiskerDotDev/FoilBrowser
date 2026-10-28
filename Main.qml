pragma ComponentBehavior : Bound;

import QtQuick;
import App.Core;
import QtWebEngine;
import QtQuick.Effects;
import QComponents;

//Consider making a helper function to return the Qt.Point value, except, with a shorter name
//example signature: pt(x, y)

Window{
  id: root;
  objectName: "ROOTWIN";
  // contentItem.smooth: false;
  property real m_tbWidth: 30;
  width: 900;
  height: 600;
  minimumHeight: root.m_tbWidth;
  minimumWidth: 450;
  // flags: Qt.Window | Qt.FramelessWindowHint; //Frameless window that doesn't close upon losing focus.
  // visibility: Window.Windowed;
  // visible: false; // set true by C++
  color: "#808080";
  Rectangle{
    id: baserect; // solely to allow shader effects to be applied across the whole window
    color: root.color;
    z: -10;
    anchors.fill: root.contentItem;
  }

  // BasicRectangle{
  // // anchors.fill: parent;
  //   width: 100; height: this.width;
  //   z: 100;
  // onMouseClicked: (buttons, pos) => console.log(buttons + pos);
  // }

  property alias backColor : root.color; //Alias "backColor" for the Window's background color
  property int tabLimit: -1; // Great for testing and performance.
  // title: (currentView?.title ?? "Browser"); //qmllint disable missing-property
  property Component webTabComponent : Qt.createComponent("WebTab.qml");
  property Component pageLoaderComponent: Qt.createComponent("PageLoaderPopup.qml");
  property Component downloadPageComponent: Qt.createComponent("DownloadPage.qml");
  property Component permissionPageComponent: Qt.createComponent("PermissionPage.qml");
  property bool enableShaders: true;

  ResizeEdges{
    id: resizeEdges;
    win: root;
    z: 4;
    verticalBounds: 2; horizontalBounds: 3;
  }

  function switchToTab(idx : int): void{
    // if(webBox.count <= idx || idx  < 0 || webBox.currentIndex === idx) return;
    // webBox.currentIndex = idx;
    // currentView.forceActiveFocus();
  }

  function deleteTab( idx  = webBox.currentIndex, spliceTabArray = true) : void{
    // Consider adjusting it to allow first tab deletion??
    // add param to enable tab to be "emptied".
    // console.log("trying to destroy", idx);
    // if ( webBox.currentIndex  === 0  && idx === 0){
    //   // currentView.url = urlHelper.getEngineURL();
    //   //Prevents first tab from being deleted. Resets it instead.
    // } else{
    //   webBox.currentIndex -=1; // Change this up!
    //   if(webBox.currentIndex < 0)webBox.currentIndex = 0;
    //   webBox.children[idx].destroy();
    //   webBox.children.splice(idx, 1); //Apparently I have to destroy the child
    //   gc();
    //   // root.tabs.splice(idx, 1); // Splicing an off-screen value. We should probably make a safeguard to temporarily disable tab switching?

    //   //after splicing, consider either adding a default/placeholder value, or, make it inaccessible.
    //   //Allow to create a default component in place?
    //   // Or, make it a custom component so it can be replaced by something else? Wouldn't that be duplicated logic?
    //   // Exclude tab?

    // }
  }

  QtObject{
    id: colorValues;
    property real saturation: 0.0;
    property real contrast: 0.0;
    property real brightness:  0.0
    property real colorization: 0.0;
    property real colorization_R: 0.0;
    property real colorization_G: 0.0;
    property real colorization_B: 0.0;

    property color colorization_color: Qt.rgba(colorValues.colorization_R,colorValues.colorization_G,colorValues.colorization_B, 1.0);
  }

  StackWidget{
    id: webBox;
    objectName: "_WebBox";
    parent: root.contentItem;
    z: 3;
    smooth: false; antialiasing: false;
    onCurrentIndexChanged: console.log(webBox.currentIndex);

    /*
    NOTE:
    The anchors and layers grouped properties are FUNCTIONAL within this type, however, the QML  Language Server is having difficulty parsing it.
*/
    anchors{
      fill: root.contentItem;
      topMargin: titleBar.height;
      leftMargin: resizeEdges.horizontalBounds;
      bottomMargin: resizeEdges.verticalBounds;
      rightMargin: resizeEdges.horizontalBounds;
    }

    layer{
      enabled: true;
      smooth: true;
      mipmap: false
      effect:MultiEffect{
        id: browserEffects;
        blurEnabled: false; autoPaddingEnabled: false; maskEnabled: false; shadowEnabled: false;
        // start drafting  Values
        brightness: colorValues.brightness;
        saturation: colorValues.saturation;
        contrast: colorValues.contrast;
        colorization: colorValues.colorization;
        colorizationColor: colorValues.colorization_color;
      }
    }

    // function handleDownload(donwreq : WebEngineDownloadRequest) : void {
    //   console.log( "Download triggered for:", donwreq.downloadFileName );
    //   var dialog =
    //       root.pageLoaderComponent.createObject(root,{win: root,
    //                                               content: root.downloadPageComponent.createObject( null, { _req : donwreq} ) }
    //                                             );
    // }

    // signal globalProfileDownloadRequested( WebEngineDownloadRequest download ); // QWebEngineDownloadRequest

    // onGlobalProfileDownloadRequested: (download)=>webBox.handleDownload(download);

    Connections {
      id: m_viewHandler;
      target: webBox.currentTab;
      // enabled: m_viewHandler.target !== null;
      // enabled: false;
      ignoreUnknownSignals: true;
      function onPermissionRequested ( permission ) {
        console.log("permission requested!");
        // if(permission.isValid !== true){console.log("invalid request!");return;}
        if(permission.permissionType === WebEnginePermission.PermissionType.Notifications){console.log("can't do notifications!");permission.deny(); return;}

        if(permission.state !== WebEnginePermission.State.Ask){console.log("already granted!"); return;}
        var dialog = root.pageLoaderComponent.createObject(root, {win: root, content: root.permissionPageComponent.createObject( null, { requestedPermission : permission} )} );
      }

      function onFullScreenRequested( request ){
        request.accept();
      }

      function onNewWindowRequested ( request ) {
        if(request.userInitiated === false){
          return;
        }else if( request.destination === WebEngineNewWindowRequest.InNewTab ){ webBox.createTab( request.requestedUrl ); }
        else if( request.destination === WebEngineNewWindowRequest.InNewBackgroundTab ){webBox.createTab(request.requestedUrl, false ); }
      }
      function onCloseRequested() { root.deleteTab(); }

      // More functions here...
      function onUrlChanged(){console.log("URL CHANGED!");searchBar.text = webBox.readPropertyCache("url", "NULL").toString(); }
    }

    function createTab(
      idx = 0,
      _url   = urlHelper.getEngineURL( ),
    focusOnCreation = true,
    overwriteTab = false,
    tabComponent = root.webTabComponent
    ) : void{
          console.log("count of children: " + webBox.children.length);
          if( ( webBox.children.length >= root.tabLimit )  && root.tabLimit !== -1) {return;}
          // let tab =  tabComponent.createObject(webBox, { url : _url});
          // webBox.registerTab(webBox.children.length -1);
          // webBox.createTab()
          webBox.createObject("webtab", focusOnCreation, {url: _url}, idx);

        }

    Component.onCompleted:{
      // webBox.setup();
      // webBox.registerComponent("WebTab.qml", "webtab");
      // webBox.createTab();
      // console.log("registered!");
      // console.log(webBox.currentTab);
      // searchBar.text = webBox.getPropertyByName(currentIndex, "url")
    }
    // onCurrentIndexChanged: console.log("IDX: "+this.currentIndex);
    // onCurrentTabChanged: ;

    // WebTab{}


  } // ![StackWidget]
  // property alias tabs: webBox.children;
  // property Item currentView: webBox.tabAtIndex(webBox.getcurrentIndex());


  Loader{
    id: colorAdjustmentMenuLDR;
    active: false;
    onActiveChanged: console.log("activity: ", active);
    sourceComponent: Component{
      PageLoaderPopup{
        win:root;
        onClosing: colorAdjustmentMenuLDR.active = false;
        transientParent: root;
        destroyOnClose: false;
        width: 300; height: 300;
        content: ColorAdjustmentPage{
          saturation_control.spinbox.onCurrentvalueChanged: colorValues.saturation = saturation_control.spinbox.currentvalue;
          contrast_control.spinbox.onCurrentvalueChanged: colorValues.contrast = contrast_control.spinbox.currentvalue;
          brightness_control.spinbox.onCurrentvalueChanged: colorValues.brightness = brightness_control.spinbox.currentvalue;
          colorization_control.spinbox.onCurrentvalueChanged: colorValues.colorization = colorization_control.spinbox.currentvalue;
          colorization_R_control.spinbox.onCurrentvalueChanged: colorValues.colorization_R = colorization_R_control.spinbox.currentvalue;
          colorization_G_control.spinbox.onCurrentvalueChanged: colorValues.colorization_G = colorization_G_control.spinbox.currentvalue;
          colorization_B_control.spinbox.onCurrentvalueChanged: colorValues.colorization_B = colorization_B_control.spinbox.currentvalue;
          Component.onCompleted:{ // we just restore color values from colorValues [QtObject]
            saturation_control.spinbox.setValue(colorValues.saturation);
            contrast_control.spinbox.setValue(colorValues.contrast);
            brightness_control.spinbox.setValue(colorValues.brightness);

            colorization_control.spinbox.setValue(colorValues.colorization);
            colorization_R_control.spinbox.setValue(colorValues.colorization_R);
            colorization_G_control.spinbox.setValue(colorValues.colorization_G);
            colorization_B_control.spinbox.setValue(colorValues.colorization_B);

          }
        }
      }

    }

  } // ![ColorAdjustmentsLoader]

  Loader{
    id: tabSwitcherLDR;
    active: false;
    sourceComponent: Component{
      PageLoaderPopup{
        win:root;
        id:switcher;
        transientParent: root;
        onClosing: tabSwitcherLDR.active = false;
        destroyOnClose: false;
        content:GridView{
          id: tabGrid
          parent: switcher.contentItem;
          anchors.fill: parent;
          anchors.topMargin: switcher.titleBar.height;
          cellWidth: Math.floor(this.width / 5);
          antialiasing: false;
          cellHeight: cellWidth;
          // model: webBox.count;
          boundsBehavior: Flickable.StopAtBounds;
          keyNavigationWraps: true;
          onCurrentIndexChanged:()=>{ if(tabGrid.currentIndex >= tabGrid.count)tabGrid.currentIndex = 0; }
          delegate: IconButton{
            required property int index;
            size: Qt.point(tabGrid.cellWidth, tabGrid.cellHeight);
            iconsrc: webBox.children[index]?.icon;
            hitbox.acceptedButtons: Qt.LeftButton | Qt.RightButton;
            hitbox.onTapped:(eventPoint, button)=>{
                              if(button === Qt.LeftButton){ console.log("switching!"); root.switchToTab(index); }
                              if(button === Qt.RightButton){ root.deleteTab(index);/* this.destroy();*/}
                            }
          }
          Keys.onReturnPressed:()=>{root.switchToTab(tabGrid.currentIndex);}
          Keys.onTabPressed: ()=>{ tabGrid.currentIndex++;}
          Keys.onEscapePressed:()=>{switcher.close();}
        }

      }
    }
  } // ![TabSwitcherLDR]


  // Base Type: Popup
  // Popup loads a Page, as a property. (property Page loadItem: null)
  // The Page must contain buttons, or, a mean to make the Popup to contain a "result" value (Q_ENUM)
  // Or, alternatively, emit a signal "ResultChosen" with the result. And, any other extra data... (e.g the reason the Popup was closed.)
  // Consider passing the request to the "page" of the dialog, and letting the dialog mostly independently decide the fate of the request.
  // For the most part, the WebEngine requests can indeed be tossed around and have an effect across the whole thing.

  // You need a DownloadPage, and, PermissionPage

  TitleBar{
    win: root;
    id: titleBar;
    height: root.m_tbWidth;
    z: 0;
    content: Rectangle{
      color: root.color;
      antialiasing: false; smooth: false;
    }

    TextButton{
      id: textBtn;
      x: 4; y: 4;
      size: Qt.point(20, 20);
      text: '#' + webBox.currentIndex;
      property WheelHandler scrollArea: WheelHandler{
        onWheel: (wheel)=>{
                   if (wheel.angleDelta.y > 0){ root.switchToTab( webBox.currentIndex + 1 ); } // Wheel up
                   else if(wheel.angleDelta.y < 0) { root.switchToTab( webBox.currentIndex - 1 ); } // Wheel down
                 }
      }
      border.width: 2;
      border.color: "#000000";
      color: root.color;
    }
    BasicButton{
      id: basicBtn;
      size: Qt.point(20,20);
      x: 30; y: 4;
      hitbox.longPressThreshold: 0.75;
      hitbox.acceptedButtons: Qt.LeftButton | Qt.RightButton;
      hitbox.onLongPressed: ()=>{webBox.layer.enabled ^=1}
      hitbox.onTapped: (eventPoint, button)=>{
                         if(button === Qt.LeftButton) colorAdjustmentMenuLDR.active ^=1;
                         if(button === Qt.RightButton) webBox.layer.enabled ^=1;
                       }
      // JUST A PLACEHOLDER!
      // maybe..?
    }

    SimpleProgressBar{
      id: progressbar;
      // width: parent.width;
      anchors{
        right: titleBar.right;
        left: titleBar.left;
        bottom: titleBar.bottom;
      }
      height: 3;


      // value:  (root.currentView?.loadProgress/100);
      enableColorSmoothing: true;
    }

    Text {
      id: text1
      x: 83
      y: 8
      text: ("total tabs: " +  webBox.children.length);
      font.pixelSize: 12
    }

    DefaultWindowControls{
      win: root;
      btnsize: Qt.point(20, 20);
      z: 1;
      closeButtonIcon: Q_FileHelper.getLocalResource("ico/close001.png", true);
    }

    StyledTextEdit{
      id: searchBar;
      y: parent.y + 3;
      height: (parent.height - 9);
      width: Math.round(root.width / 2);
      x: (Math.round( parent.width / 2 ) - Math.round( width / 2 ) );
      // text: ;
      renderType: TextEdit.NativeRendering;
      color:"#303030";
      textcolor: "#FFFFFF";
      onEditingFinished: ()=>{if(searchBar.text !== ""){
                             // webBox.currentTab.url = urlHelper.getURL(searchBar.text);
                             // webBox.forceActiveFocus();
                           }
                         }
    }

    // Rectangle{
    //   id:generic_rect;
    //   height:searchBar.height;
    //   width: 150;
    //   x: searchBar.x + searchBar.width;
    //   y: searchBar.y;
    //   gradient: Gradient{
    //     orientation: Gradient.Horizontal;
    //   GradientStop{position: 0.0; color:"#FF0000";}
    //   GradientStop{position: 0.5; color: "#00FF00";}
    //   GradientStop{position: 1.0; color: "#0000FF";}
    //   }
    // }

    // BorderImage{
    //   id: img;
    //   height:searchBar.height;
    //   width: 150;
    //   x: searchBar.x + searchBar.width;
    //   y: searchBar.y;
    //   source: Q_FileHelper.getLocalResource("ASSETS/BORDERIMAGES/retro-pressed.png", true);
    //   horizontalTileMode: BorderImage.Repeat; verticalTileMode: BorderImage.Repeat;
    //   border.bottom: 5; border.top: 5; border.left: 5; border.right: 5;
    // }

  } // ![TitleBar]

  Shortcut{
    sequence: "Ctrl+E";
    onActivated:root.enableShaders ^=1;
  }
  Shortcut {
    sequence: "Ctrl+L";
    onActivated: {
      searchBar.content.forceActiveFocus();
      searchBar.content.selectAll();
    }
  }

  Shortcut{
    sequence: "Ctrl+Tab";
    onActivated: {tabSwitcherLDR.active ^=1}}
  Shortcut{
    sequence: "Ctrl+T";
    onActivated: {webBox.createTab(webBox.currentIndex +1);}}
  Shortcut{
    sequence: "Ctrl+W";
    onActivated: {root.deleteTab();}}
  Shortcut{
    sequence: "Ctrl+R";
    onActivated: ()=>{/*root.currentView?.reload();*/ gc();}}

  Shortcut{
    sequence: "Ctrl+Shift+R";
    onActivated: ()=>{/*root.currentView?.reloadAndBypassCache(); */gc();}} // try a signal to request reloading!

  Shortcut{
    sequence: "Ctrl+1";
    onActivated: {root.switchToTab(0);}}
  Shortcut{
    sequence: "Ctrl+2";
    onActivated: {root.switchToTab(1);}}
  Shortcut{
    sequence: "Ctrl+3";
    onActivated: {root.switchToTab(2);}}
  Shortcut{
    sequence: "Ctrl+4";
    onActivated: {root.switchToTab(3);}}
  Shortcut{
    sequence: "Ctrl+5";
    onActivated: {root.switchToTab(4);}}
  Shortcut{
    sequence: "Ctrl+6";
    onActivated: {root.switchToTab(5);}}
  Shortcut{
    sequence: "Ctrl+7";
    onActivated: {root.switchToTab(6);}}
  Shortcut{
    sequence: "Ctrl+8";
    onActivated: {root.switchToTab(7);}}
  Shortcut{
    sequence: "Ctrl+9";
    onActivated: {root.switchToTab(8);}}

  Loader{
    active: root.enableShaders;

    sourceComponent: Component{
      ShaderEffectSource{
        id:effect_src;
        parent: root.contentItem;
        z: 10;
        sourceItem: root.contentItem;
        mipmap: false; recursive: false;
        // antialiasing and whatnot?
        ShaderEffect{
          id: effect
          parent: root.contentItem;
          visible: true; anchors.fill: parent;
          z: 10;
          property variant source: effect_src;
          smooth: root.contentItem; antialiasing: root.contentItem.antialiasing;
          property int depth_R: 3;
          property int depth_G: 3;
          property int depth_B: 2;
          fragmentShader: Q_FileHelper.getLocalResource("shaders/shader.frag.qsb", true);
        }
      }// ![Effect source]
    }// ![Component Wrapper]

  }

  FPSCounter{
    id: fpsTimer;
    interval: 1000;
    window: root;
    // active: true;
    // onFpsChanged: console.log(fpsTimer.fps);

  } // keeping this here in case...

  ImageSource{
  id: src;
  }
  CPUImage{
  id: img;
  antialiasing: false;
  // smooth: false;
  width: 100;
  height:100;
  z: 100;
  Timer{
  interval: 1;
  running: true;
  repeat: true;
  onTriggered:img.x +=1;
  }

  imgsrc: src;
  }

  // QML_FPSCounter{
  // id: qmlFPSTIMER;
  // win: root;
  // logFramerateToConsole: true;
  // }

  Component.onCompleted: {
    // console.log(src);
    root.requestActivate();
    // fpsTimer.start();
    // qmlFPSTIMER.start();
    // console.log(viewHandler.active);
    // console.log(downloadHandler.active);
  }
} // ![WND]
