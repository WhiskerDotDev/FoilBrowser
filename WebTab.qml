import QtQuick;
import QtWebEngine;

WebEngineView{
  id: mainView;
  visible: false;
  Timer{
    id: m_deepSleepTimer
    interval: 30000; // we can make this configurable
    running: !(mainView.visible || mainView.recentlyAudible);
    triggeredOnStart: false;
    onTriggered:
        ()=>{
          if(mainView.lifecycleState !== WebEngineView.LifecycleState.Discarded){
            mainView.visible = false;
            mainView.lifecycleState = WebEngineView.LifecycleState.Discarded;}
        }
  } property alias deepSleepTimer : m_deepSleepTimer;
  // ![TIMER]

  onVisibleChanged:()=>{
                     // console.log("visibility changed!");
                     if( (mainView.visible || mainView.recentlyAudible) === false )
                     { mainView.lifecycleState = WebEngineView.LifecycleState.Frozen; }
                     else{ mainView.lifecycleState = WebEngineView.LifecycleState.Active; }
                   }
  onRecentlyAudibleChanged:()=>{
                             if( (mainView.visible || mainView.recentlyAudible) === false )mainView.lifecycleState = WebEngineView.LifecycleState.Frozen; }
  smooth: false; antialiasing: false; // Let's try these? We can always change them later.
  zoomFactor: 1.0;
  settings.spatialNavigationEnabled: true;

}
