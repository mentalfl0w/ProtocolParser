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

    property int max_msg_num: 10
    property alias delegate: message_list.delegate
    property alias message_model: message_model
    property alias view: message_list

    ListModel{
        id: message_model
        property int begin_index: 0
        property int end_index: 0
        onCountChanged: {
            if (end_index == count - 2 || begin_index == end_index)
            {
                while(show_model.count > max_msg_num)
                {
                    show_model.remove(0)
                    begin_index++
                }
                show_model.append(message_model.get(count - 1))
                end_index = count-1
                //console.log('append:',begin_index,end_index)
            }
        }
    }
    ListModel{
        id: show_model
    }

    ListView{
        id: message_list
        Layout.alignment: Qt.AlignHCenter
        Layout.preferredHeight: contentHeight
        Layout.preferredWidth: parent.width
        interactive: false
        model: show_model
        add: Transition {
            NumberAnimation { properties: "y"; from: message_list.height; duration: 200 }
        }
        displaced: Transition {
            NumberAnimation { properties: "y"; duration: 200 }
        }
    }
    Connections{
        target: view
        function onPull_up_triggered()
        {
            for(let i = message_model.begin_index - 1, count = 0;
                i >= 0 && count < max_msg_num; i--,count++ )
            {
                show_model.insert(0,message_model.get(i))
                message_model.begin_index = i
                if (show_model.count > max_msg_num + 2)
                {
                    show_model.remove(show_model.count - 1)
                    message_model.end_index--
                }
            }
            //console.log('up:',message_model.begin_index,message_model.end_index)
        }
        function onPull_down_triggered()
        {
            for(let i = message_model.end_index + 1, count = 0;
                i < message_model.count && count < max_msg_num; i++,count++ )
            {
                show_model.append(message_model.get(i))
                message_model.end_index = i
                if (show_model.count > max_msg_num)
                {
                    show_model.remove(0)
                    message_model.begin_index++
                }
            }
            //console.log('down:',message_model.begin_index,message_model.end_index)
        }
    }
}
