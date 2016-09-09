/*
  Copyright (C) 2016 GiuliettaSW.
  Contact: tortoisedoc <users.giulietta@gmail.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick 2.0
import Sailfish.Silica 1.0
import "data.js" as Data

Page {
    id: page

    property alias mimes_view:mimesView;

    ListModel{
        id:mimesModel;
    }

    SilicaListView{
        id:mimesView;
        model:mimesModel;
        anchors.fill:parent;
        anchors.leftMargin: 10;        
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        highlightFollowsCurrentItem:true;
        highlightMoveDuration:0;
        delegate: Item{
            id:itemDelegate;
            width:parent.width;
            height : Theme.iconSizeMedium;
            Row{
                anchors.fill: parent;
                Item{
                    width:parent.width;
                    height:parent.height;
                    Image{
                        id:mimeIcon;
                        width:Theme.iconSizeMedium;
                        height:Theme.iconSizeMedium;
                        source:icon;
                    }
                    Label{
                        width:parent.width - mimeIcon.width;
                        height:parent.height;
                        text:mime;
                    }
                }
            }
            MouseArea{
                anchors.fill:parent;
                onPressAndHold:{
                    Data.selectedObject = mimesModel.get(mimesView.currentIndex);
                    console.log(Data.selectedObject)
                    pageStack.push(
                        Qt.resolvedUrl("SecondPage.qml"))
                }
                onEntered:{
                    mimesView.currentIndex = index;
                }
            }
        }
    }

    Connections{
        target:MimeCache;
        onCacheUpdated:{
            console.debug("CACHE UPDATED");
            mimesModel.clear();
            for(var i = 0; i < MimeCache.keys.length; i++) {
                var key = MimeCache.keys[i];
                var imagePaths = MimeCache.getIconPathsForMime(key);
                var defIcon="";
                if (imagePaths.len > 0){
                    defIcon = imagePaths[0];
                }
                mimesModel.append({
                                      "index": i,
                                      "mime": key,
                                      "icon": defIcon
                                  });
            }
        }
    }
/*
    // To enable PullDownMenu, place our content in a SilicaFlickable
    SilicaFlickable {
        anchors.fill: parent

        // PullDownMenu and PushUpMenu must be declared in SilicaFlickable, SilicaListView or SilicaGridView
        PullDownMenu {
            MenuItem {
                text: qsTr("Show Page 2")
                onClicked: pageStack.push(Qt.resolvedUrl("SecondPage.qml"))
            }
        }

        // Tell SilicaFlickable the height of its content.
        contentHeight: column.height

        // Place our content in a Column.  The PageHeader is always placed at the top
        // of the page, followed by our content.
        Column {
            id: column

            width: page.width
            spacing: Theme.paddingLarge
            PageHeader {
                title: qsTr("UI Template")
            }
            Label {
                x: Theme.paddingLarge
                text: qsTr("Hello Sailors")
                color: Theme.secondaryHighlightColor
                font.pixelSize: Theme.fontSizeExtraLarge
            }
        }
    }
*/
}


