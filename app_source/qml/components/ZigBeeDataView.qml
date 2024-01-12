import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import RibbonUI
import ProtocolParser
import "."

Item{
    id: control
    property bool show_tooltip: false
    property bool auto_scroll_to_bottom: false
    property double top_padding: 0
    property double bottom_padding: 0
    property bool handle_serial: true

    Connections{
        target: SerialPortManager
        function onOpenedChanged(){
            if (handle_serial&&SerialPortManager.opened)
            {
                SerialPortManager.handledBymanager = false
            }
            else
                SerialPortManager.handledBymanager = true
        }
    }

    onHandle_serialChanged: {
        if (handle_serial)
        {
            SerialPortManager.handledBymanager = false
        }
        else
        {
            SerialPortManager.handledBymanager = true
        }
    }

    onAuto_scroll_to_bottomChanged: {
        if(auto_scroll_to_bottom)
        {
            identify_view.scroll_to_bottom()
            data_view.scroll_to_bottom()
        }
    }
    Item{
        anchors{
            top:parent.top
            left: parent.left
        }
        width: parent.width / 2
        height: parent.height
        RibbonMessageListView{
            id: identify_view
            anchors.fill: parent
            top_padding: control.top_padding + identify_title_bar.height + (!RibbonTheme.modern_style ? 10 : 0)
            bottom_padding: control.bottom_padding
            delegate: ZigBeeMessage{
                show_tooltip: control.show_tooltip
                component_width: identify_view.width / 2
            }
            view.onHeightChanged: {
                if (control.auto_scroll_to_bottom)
                    scroll_to_bottom()
            }
            Event{
                id:identify_view_event
                type: "zigbee_identify_data_view"
                onTriggered: function(data){
                    identify_view.message_model.append(data)
                }
            }
            Component.onCompleted: {
                EventsBus.reg_event(identify_view_event)
            }
        }
        RibbonRectangle{
            id: identify_title_bar
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: control.top_padding + (!RibbonTheme.modern_style ? 10 : 0)
            implicitWidth: parent.width - 20
            implicitHeight: identify_title.contentHeight + 10
            radius: 5
            color: RibbonTheme.dark_mode ? Qt.rgba(0, 0, 0, 0.5) : Qt.rgba(255, 255, 255, 0.5)
            RibbonText {
                id: identify_title
                anchors.centerIn: parent
                text: "认证信息流"
                color: RibbonTheme.dark_mode ? "white" : "black"
                font.pixelSize: 15
                view_only: true
            }
        }
    }
    Item{
        anchors{
            top:parent.top
            right: parent.right
        }
        width: parent.width / 2
        height: parent.height
        RibbonMessageListView{
            id: data_view
            top_padding: control.top_padding + data_title_bar.height
            bottom_padding: bottom_bar.height
            width: parent.width / 2
            delegate: ZigBeeMessage{
                show_tooltip: control.show_tooltip
                component_width: data_view.width / 2
            }
            view.onHeightChanged: {
                if (control.auto_scroll_to_bottom)
                    scroll_to_bottom()
            }
            Event{
                id:data_view_event
                type: "zigbee_recv_data_view"
                onTriggered: function(data){
                    data_view.message_model.append(data)
                }
            }
            Component.onCompleted: {
                EventsBus.reg_event(data_view_event)
            }

            // Component.onCompleted: {
            //     for(let i = 0; i<20;i++)
            //     {
            //         function generate_value(){
            //             let repeat_count = Math.floor(Math.random()*40) + 6
            //             let t = `FF ${repeat_count.toString(16).toUpperCase()} ${Math.floor(Math.random()*16).toString(16).toUpperCase()}${Math.floor(Math.random()*16).toString(16).toUpperCase()} ${Math.floor(Math.random()*16).toString(16).toUpperCase()}${Math.floor(Math.random()*16).toString(16).toUpperCase()} ${Math.floor(Math.random()*16).toString(16).toUpperCase()}${Math.floor(Math.random()*16).toString(16).toUpperCase()} ${Math.floor(Math.random()*16).toString(16).toUpperCase()}${Math.floor(Math.random()*16).toString(16).toUpperCase()}`
            //             for (let j=0;j<repeat_count;j++){
            //                 t+=` ${Math.floor(Math.random()*16).toString(16).toUpperCase()}${Math.floor(Math.random()*16).toString(16).toUpperCase()}`
            //             }
            //             t+=" FE"
            //             return t
            //         }
            //         if (Math.floor(Math.random()*2) === 0)
            //         {
            //             let v = generate_value()
            //             let sender = v.split(" ").slice(4, 6).join("")
            //             message_model.append({
            //                                      text: v,
            //                                      recieved: true,
            //                                      sender: sender
            //                                  })
            //         }
            //         else
            //         {
            //             let v = generate_value()
            //             let sender = v.split(" ").slice(4, 6).join("")
            //             message_model.append({
            //                                      text: v,
            //                                      decrypted_text: v,
            //                                      recieved: true,
            //                                      sender: sender
            //                                  })
            //         }
            //     }
            // }
        }
        RibbonRectangle{
            id: data_title_bar
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.top
            anchors.topMargin: control.top_padding + (!RibbonTheme.modern_style ? 10 : 0)
            implicitWidth: parent.width - 20
            implicitHeight: data_title.contentHeight + 10
            radius: 5
            color: RibbonTheme.dark_mode ? Qt.rgba(0, 0, 0, 0.5) : Qt.rgba(255, 255, 255, 0.5)
            RibbonText {
                id: data_title
                anchors.centerIn: parent
                text: "接收数据流"
                color: RibbonTheme.dark_mode ? "white" : "black"
                font.pixelSize: 15
                view_only: true
            }
        }
    }

    function clear_all_message()
    {
        identify_view.message_model.clear()
        data_view.message_model.clear()
    }
}
