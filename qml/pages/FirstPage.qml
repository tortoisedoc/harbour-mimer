import QtQuick 2.0
import Sailfish.Silica 1.0
import "../backend.js" as Backend

Page {
    id: page

    ListModel{
        id:mimesModel;
    }

    SilicaListView{
        id:mimes;
        model:mimesModel;
        anchors.fill:parent;
        anchors.leftMargin: 10;
        delegate: Item{
            width:parent.width;
            height : Theme.iconSizeMedium;
            MouseArea{
                anchors.fill: parent;
                onClicked: {                    
                    Backend.mime = mime;
                    console.log(" MIME " + Backend.mime)
                    defaultAppsModel.loadAppsFromMime(mime);
                    userDefaultAppsModel.loadAppsFromMime(mime, true);
                    pageStack.push(Qt.resolvedUrl("SecondPage.qml"))
                }
            }
            Row{
                anchors.fill: parent;
                spacing:10;
                Image{
                    id:mimeIcon;
                    source:icon;
                    width:Theme.iconSizeMedium;
                    height:Theme.iconSizeMedium;
                }
                Label{
                    //width:parent.width - mimeIcon.width;
                    height:parent.height;
                    text:mime;
                    //font.pixelSize: 13;
                }
            }
        }
    }

    Connections{
        target:MimeCache;
        onCacheUpdated:{
            mimesModel.clear();
            for(var i = 0; i < MimeCache.keys.length; i++) {
                var key = MimeCache.keys[i];
                var imagePaths = MimeCache.getIconPathsForMime(key);
                var defIcon="";                
                if (imagePaths.length > 0){
                    defIcon = imagePaths[0];
                }else defIcon = "";
                mimesModel.append({
                                      "mime": key,
                                      "icon": defIcon
                                  });
            }
            console.debug("CACHE UPDATED");
        }
    }
    Component.onCompleted: {
        appsModel.loadAppsFromPath("/usr/share/applications")
    }
}


