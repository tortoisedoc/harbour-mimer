import QtQuick 2.0
import Sailfish.Silica 1.0
import "../"
import "../backend.js" as Backend

Page {
    id: page

    Column{
        spacing:30;
        anchors.fill: parent;

        DesktopEntryList{
            model:userDefaultAppsModel;
            titleText:qsTr("User-defined");
            width:parent.width;
            height:Theme.iconSizeLarge;
            delegate: DesktopEntryDelegate{
                id :myDelegate;
                width:parent.width;
                height:Theme.iconSizeMedium;
                image:icon;
                text:name;
                onClicked:{                   
                    console.log(" MIME  2 "+ index);
                    Backend.currentSelection = index;
                    pageStack.push(Qt.resolvedUrl("AppsPage.qml"));
                }
            }
        }

        Button{
            visible:userDefaultAppsModel.count == 0;
            text:qsTr("Add User Default..")
            width:parent.width;
            height : Theme.iconSizeMedium;
            onClicked: {
                Backend.currentSelection = null;
                pageStack.push(Qt.resolvedUrl("AppsPage.qml"));
            }
        }

        DesktopEntryList{
            model:defaultAppsModel;
            titleText:qsTr("System Handlers");
            width:parent.width;
            height:Theme.iconSizeLarge;
        }
    }
}





