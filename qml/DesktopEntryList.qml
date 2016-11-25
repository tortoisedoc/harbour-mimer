import QtQuick 2.0
import Sailfish.Silica 1.0
import "./"

SilicaListView{
    id: defaultApps
    property string titleText: "";
    anchors.topMargin: 20;

    header: PageHeader {
        id:myHeader;
        title: defaultApps.titleText;
        width:parent.width;
        height:Theme.iconSizeMedium;
    }
    delegate: DesktopEntryDelegate{
        id :myDelegate;
        width:parent.width;
        height:Theme.iconSizeMedium;
        image:icon;
        text:name;
    }
    VerticalScrollDecorator {}
}
