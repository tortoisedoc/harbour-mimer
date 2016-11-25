import QtQuick 2.0
import Sailfish.Silica 1.0

BackgroundItem {
    property alias image: appIcon.source;
    property alias text: appLabel.text;
    id: delegate    
    Row{
        anchors.leftMargin: 10;
        anchors.fill: parent;
        spacing:10;

        Image {
            id: appIcon
            source: "";
            width:Theme.iconSizeMedium;
            height:Theme.iconSizeMedium;
        }
        Label {
            id: appLabel
            x: Theme.paddingLarge
            text: "";
            anchors.verticalCenter: parent.verticalCenter;
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere;
            color: delegate.highlighted ?
                       Theme.highlightColor : Theme.primaryColor
        }
    }    
}
