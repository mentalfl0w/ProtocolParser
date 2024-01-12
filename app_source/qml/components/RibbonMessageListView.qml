import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import RibbonUI

RibbonView{
    id: view
    anchors{
        top: parent.top
        bottom: parent.bottom
    }
    spacing: 0

    property alias delegate: message_list.delegate
    property alias message_model: message_model
    property alias view: message_list

    ListModel{
        id: message_model
    }

    ListView{
        id: message_list
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredHeight: contentHeight
        Layout.preferredWidth: parent.width
        interactive: false
        model: message_model
        add: Transition {
            NumberAnimation { properties: "x,y"; from: message_list.height; duration: 1000 }
        }
    }
}
