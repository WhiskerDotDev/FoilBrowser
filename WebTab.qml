import QtQuick;
import QtWebEngine;

WebEngineView{
  id: mainView;
  visible: false;
  property Timer deepSleepTimer: Timer{
    interval: 30000; // we can make this configurable
    running: !(mainView.visible);
    triggeredOnStart: false;
    onTriggered:
        ()=>{
          if(mainView.lifecycleState !== WebEngineView.LifecycleState.Discarded){
            mainView.visible = false;
            mainView.lifecycleState = WebEngineView.LifecycleState.Discarded;}
        }
  }// ![TIMER]

  // profile.onDownloadRequested: console.log(mainView + "has requested a download!");

  onVisibleChanged:()=>{
                     // console.log("visibility changed!");
                     if( (mainView.visible === 0) && (mainView.recentlyAudible === 0) )
                     { mainView.lifecycleState = WebEngineView.LifecycleState.Frozen; }
                     else{ mainView.lifecycleState = WebEngineView.LifecycleState.Active; }
                   }
  onRecentlyAudibleChanged:()=>{
                             if( (mainView.visible === 0) && (mainView.recentlyAudible === 0) )mainView.lifecycleState = WebEngineView.LifecycleState.Frozen; }
  smooth: false; antialiasing: false; // Let's try these? We can always change them later.
  zoomFactor: 1.0;
  settings.spatialNavigationEnabled: true;

}
