import QtQuick
import QtQuick.Layouts
import RibbonUI

/* 仅适用于DL-LN3X系列 */

Flow{
    id: control
    property alias text: label.text
    property bool show_text: true
    property bool dark_mode: RibbonTheme.dark_mode
    property bool show_tooltip: true
    property bool is_recieved: false
    property double padding: 5
    property int font_size: 13
    property double component_width: 300
    property int data_width: component_width - (show_text ? label.width : 0) - z_head.width - z_length.width - z_src_port.width -
                             z_des_port.width - z_address.width - z_tail.width - spacing*(show_text ? 13 : 12) - 6
    property var data_list


    RibbonText {
        id: label

        height: z_data.height + control.padding * 2
        color: control.dark_mode ? "white" : control.is_recieved ? "black" : "white"
        font.pixelSize: control.font_size
        visible: control.show_text
        horizontalAlignment: RibbonText.AlignHCenter
        verticalAlignment: RibbonText.AlignVCenter
    }

    RibbonRectangle{
        radius: 5
        topRightRadius: 0
        bottomRightRadius: topRightRadius
        implicitWidth: z_head.contentWidth + control.padding * 2
        implicitHeight: z_data.height + control.padding * 2

        color: z_head_hh.hovered ? control.is_recieved ? control.dark_mode ? "#262626" : "#F0F0F0" : control.dark_mode ? "#313131" : "#6099AA" : "transparent"
        RibbonText {
            id: z_head
            anchors.centerIn: parent
            color: control.dark_mode ? "white" : control.is_recieved ? "black" : "white"
            text: typeof(data_list[0]) !== "undefined" ? data_list[0] : ""
            font.pixelSize: control.font_size
            horizontalAlignment: RibbonText.AlignHCenter
            verticalAlignment: RibbonText.AlignVCenter
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }

        RibbonToolTip{
            text: "ZigBee帧 头"
            visible: z_head_hh.hovered && control.show_tooltip
        }

        HoverHandler{
            id: z_head_hh
        }
    }


    Item {
        height: z_data.height + control.padding * 2
        width: 1
        Rectangle{
            anchors.verticalCenter: parent.verticalCenter
            width: 1
            height: z_data.height - control.padding * 2
            color: control.dark_mode ? "#525252" : "#D4D4D4"
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }
    }

    RibbonRectangle{
        radius: 0
        color: z_length_hh.hovered ? control.is_recieved ? control.dark_mode ? "#262626" : "#F0F0F0" : control.dark_mode ? "#313131" : "#6099AA" : "transparent"
        implicitWidth: z_length.contentWidth + control.padding * 2
        implicitHeight: z_data.height + control.padding * 2

        RibbonText {
            id: z_length
            anchors.centerIn: parent
            color: control.dark_mode ? "white" : control.is_recieved ? "black" : "white"
            text: typeof(data_list[0]) !== "undefined" ? data_list[1] : ""
            font.pixelSize: control.font_size
            horizontalAlignment: RibbonText.AlignHCenter
            verticalAlignment: RibbonText.AlignVCenter
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }

        RibbonToolTip{
            text: "ZigBee帧 长"
            visible: z_length_hh.hovered && control.show_tooltip
        }

        HoverHandler{
            id: z_length_hh
        }
    }

    Item {
        height: z_data.height + control.padding * 2
        width: 1
        Rectangle{
            anchors.verticalCenter: parent.verticalCenter
            width: 1
            height: z_data.height - control.padding * 2
            color: control.dark_mode ? "#525252" : "#D4D4D4"
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }
    }

    RibbonRectangle{
        radius: 0
        color: z_src_port_hh.hovered ? control.is_recieved ? control.dark_mode ? "#262626" : "#F0F0F0" : control.dark_mode ? "#313131" : "#6099AA" : "transparent"
        implicitWidth: z_src_port.contentWidth + control.padding * 2
        implicitHeight: z_data.height + control.padding * 2


        RibbonText {
            id: z_src_port
            anchors.centerIn: parent
            color: control.dark_mode ? "white" : control.is_recieved ? "black" : "white"
            text: typeof(data_list[0]) !== "undefined" ? data_list[2] : ""
            font.pixelSize: control.font_size
            horizontalAlignment: RibbonText.AlignHCenter
            verticalAlignment: RibbonText.AlignVCenter
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }

        RibbonToolTip{
            text: "ZigBee帧 源端口"
            visible: z_src_port_hh.hovered && control.show_tooltip
        }

        HoverHandler{
            id: z_src_port_hh
        }
    }

    Item {
        height: z_data.height + control.padding * 2
        width: 1
        Rectangle{
            anchors.verticalCenter: parent.verticalCenter
            width: 1
            height: z_data.height - control.padding * 2
            color: control.dark_mode ? "#525252" : "#D4D4D4"
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }
    }

    RibbonRectangle{
        radius: 0
        color: z_des_port_hh.hovered ? control.is_recieved ? control.dark_mode ? "#262626" : "#F0F0F0" : control.dark_mode ? "#313131" : "#6099AA" : "transparent"
        implicitWidth: z_des_port.contentWidth + control.padding * 2
        implicitHeight: z_data.height + control.padding * 2


        RibbonText {
            id: z_des_port
            anchors.centerIn: parent
            color: control.dark_mode ? "white" : control.is_recieved ? "black" : "white"
            text: typeof(data_list[0]) !== "undefined" ? data_list[3] : ""
            font.pixelSize: control.font_size
            horizontalAlignment: RibbonText.AlignHCenter
            verticalAlignment: RibbonText.AlignVCenter
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }

        RibbonToolTip{
            text: "ZigBee帧 目的端口"
            visible: z_des_port_hh.hovered && control.show_tooltip
        }

        HoverHandler{
            id: z_des_port_hh
        }
    }

    Item {
        height: z_data.height + control.padding * 2
        width: 1
        Rectangle{
            anchors.verticalCenter: parent.verticalCenter
            width: 1
            height: z_data.height - control.padding * 2
            color: control.dark_mode ? "#525252" : "#D4D4D4"
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }
    }

    RibbonRectangle{
        radius: 0
        color: z_address_hh.hovered ? control.is_recieved ? control.dark_mode ? "#262626" : "#F0F0F0" : control.dark_mode ? "#313131" : "#6099AA" : "transparent"
        implicitWidth: z_address.contentWidth + control.padding * 2
        implicitHeight: z_data.height + control.padding * 2

        RibbonText {
            id: z_address
            anchors.centerIn: parent
            color: control.dark_mode ? "white" : control.is_recieved ? "black" : "white"
            text: typeof(data_list[0]) !== "undefined" ? data_list.slice(4,6).join(" ") : ""
            font.pixelSize: control.font_size
            horizontalAlignment: RibbonText.AlignHCenter
            verticalAlignment: RibbonText.AlignVCenter
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }

        RibbonToolTip{
            text: "ZigBee帧 源地址"
            visible: z_address_hh.hovered && control.show_tooltip
        }

        HoverHandler{
            id: z_address_hh
        }
    }

    Item {
        height: z_data.height + control.padding * 2
        width: 1
        Rectangle{
            anchors.verticalCenter: parent.verticalCenter
            width: 1
            height: z_data.height - control.padding * 2
            color: control.dark_mode ? "#525252" : "#D4D4D4"
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }
    }

    RibbonRectangle{
        radius: 0
        color: z_data_hh.hovered ? control.is_recieved ? control.dark_mode ? "#262626" : "#F0F0F0" : control.dark_mode ? "#313131" : "#6099AA" : "transparent"
        implicitWidth: z_data.width + control.padding * 2
        implicitHeight: z_data.height + control.padding * 2

        RibbonText {
            id: z_data
            anchors.centerIn: parent
            color: control.dark_mode ? "white" : control.is_recieved ? "black" : "white"
            width: implicitWidth < control.data_width ? implicitWidth : control.data_width
            wrapMode: RibbonText.Wrap
            text: typeof(data_list[0]) !== "undefined" ? data_list.slice(6,data_list.length-1).join(" ") : ""
            font.pixelSize: control.font_size
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }

        RibbonToolTip{
            text: "ZigBee帧 数据"
            visible: z_data_hh.hovered && control.show_tooltip
        }

        HoverHandler{
            id: z_data_hh
        }
    }

    Item {
        height: z_data.height + control.padding * 2
        width: 1
        Rectangle{
            anchors.verticalCenter: parent.verticalCenter
            width: 1
            height: z_data.height - control.padding * 2
            color: control.dark_mode ? "#525252" : "#D4D4D4"
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }
    }

    RibbonRectangle{
        radius: parent.radius ? parent.radius / 2 : 5
        color: z_tail_hh.hovered ? control.is_recieved ? control.dark_mode ? "#262626" : "#F0F0F0" : control.dark_mode ? "#313131" : "#6099AA" : "transparent"
        topLeftRadius: 0
        bottomLeftRadius: topLeftRadius
        implicitWidth: z_tail.contentWidth + control.padding * 2
        implicitHeight: z_data.height + control.padding * 2

        RibbonText {
            id: z_tail
            anchors.centerIn: parent
            color: control.dark_mode ? "white" : control.is_recieved ? "black" : "white"
            text: typeof(data_list[0]) !== "undefined" ? data_list[data_list.length-1] : ""
            font.pixelSize: control.font_size
            horizontalAlignment: RibbonText.AlignHCenter
            verticalAlignment: RibbonText.AlignVCenter
            Behavior on color {
                ColorAnimation {
                    duration: 60
                    easing.type: Easing.OutSine
                }
            }
        }

        RibbonToolTip{
            text: "ZigBee帧 尾"
            visible: z_tail_hh.hovered && control.show_tooltip
        }

        HoverHandler{
            id: z_tail_hh
        }
    }
}

