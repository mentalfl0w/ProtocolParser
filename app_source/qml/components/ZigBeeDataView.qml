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
        identify_view.auto_scroll_to_bottom = auto_scroll_to_bottom
        data_view.auto_scroll_to_bottom = auto_scroll_to_bottom
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
            auto_scroll_to_bottom: control.auto_scroll_to_bottom
            delegate: ZigBeeMessage{
                show_tooltip: control.show_tooltip
                component_width: identify_view.width / 2
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
            anchors.fill: parent
            top_padding: control.top_padding + data_title_bar.height + (!RibbonTheme.modern_style ? 10 : 0)
            bottom_padding: bottom_bar.height
            delegate: ZigBeeMessage{
                show_tooltip: control.show_tooltip
                component_width: data_view.width / 2
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
