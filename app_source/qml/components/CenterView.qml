import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import RibbonUI
import ProtocolParser

SwipeView{
    id: swipeview
    currentIndex: 0
    property var tab_bar
    property var bottom_bar
    property alias zigbee_dataview: zigbee_dataview
    property alias serial_assistant: serial_assistant

    ZigBeeDataView{
        id: zigbee_dataview
        show_tooltip: tab_bar.show_tooltip
        auto_scroll_to_bottom: tab_bar.auto_scroll_to_bottom
        top_padding: tab_bar.height
        bottom_padding: bottom_bar.height
        handle_serial: swipeview.currentIndex === 0
        Rectangle{
            color: "transparent"
            anchors{
                right: parent.right
                verticalCenter: parent.verticalCenter
            }
            width: 30
            height: parent.height
            HoverHandler{
                id: right_chevron_area_hh
            }
            RibbonButton{
                anchors{
                    right: parent.right
                    verticalCenter: parent.verticalCenter
                }
                show_bg: false
                ribbon_icon.icon_size: 20
                tip_text: "前往串口助手"
                width: parent.width
                height: parent.height * 0.1
                icon_source: RibbonIcons.ChevronCircleRight
                visible: right_chevron_area_hh.hovered || hovered
                onClicked: swipeview.incrementCurrentIndex()
            }
        }
    }
    SerialPortAssistant{
        id: serial_assistant
        show_tooltip: tab_bar.show_tooltip
        auto_scroll_to_bottom: tab_bar.auto_scroll_to_bottom
        top_padding: tab_bar.height
        bottom_padding: bottom_bar.height
        handle_serial: swipeview.currentIndex === 1
        Rectangle{
            color: "transparent"
            anchors{
                left: parent.left
                verticalCenter: parent.verticalCenter
            }
            width: 30
            height: parent.height
            HoverHandler{
                id: left_chevron_area_hh
            }
            RibbonButton{
                anchors{
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
                show_bg: false
                ribbon_icon.icon_size: 20
                tip_text: "前往ZigBee数据流"
                width: parent.width
                height: parent.height * 0.1
                icon_source: RibbonIcons.ChevronCircleLeft
                visible: left_chevron_area_hh.hovered || hovered
                onClicked: swipeview.decrementCurrentIndex()
            }
        }
    }
}
