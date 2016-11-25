import QtQuick 2.0
import Sailfish.Silica 1.0
import "../"
import "../backend.js" as Backend

Page {
    id: page

    DesktopEntryList{
        id : myEntry;
        model:appsModel;
        titleText: qsTr("All apps");
        anchors.fill: parent;
        delegate: DesktopEntryDelegate{
            id :myDelegate;
            width:parent.width;
            height:Theme.iconSizeMedium;
            image:icon;
            text:name;
            onClicked:{
                if (Backend.currentSelection !== null){
                    userDefaultAppsModel.setData(
                                Backend.currentSelection,
                                desktopFile,
                                Backend.mime);
                }else
                    userDefaultAppsModel.setData(
                                -1,
                                desktopFile,
                                Backend.mime);
                MimeCache.uncacheMime(Backend.mime);
                MimeCache.cacheUpdated();
                pageStack.pop();
            }
        }
    }
}
