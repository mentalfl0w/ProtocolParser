import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import RibbonUI

RibbonMessage{
    id: message_style
    sender_text: data_model.recieved ? `${data_model.time} Node 0x${data_model.sender}` : `${data_model.time} Computer 0x${data_model.sender}`
    property var text_list: data_model.text ? data_model.text.split(' ') : []
    property var decrypted_text_list: data_model.decrypted_text ? data_model.decrypted_text.split(' ') : []
    property bool show_tooltip: true
    property double component_width: 300
    ZigBeeFrame{
        id: message_raw_layout
        text: "原始数据："
        show_text: model.recieved
        show_tooltip: message_style.show_tooltip
        data_list: message_style.text_list
        is_recieved: model.recieved
        component_width: message_style.component_width
        font_size: message_style.font_size
        visible: model.text ? true : false
    }

    Rectangle{
        Layout.alignment: Qt.AlignHCenter
        implicitWidth: parent.width - 20
        implicitHeight: 1
        color: RibbonTheme.dark_mode ? "#525252" : "#D4D4D4"
        visible: message_decrypted_layout.visible
        Behavior on color {
            ColorAnimation {
                duration: 60
                easing.type: Easing.OutSine
            }
        }
    }

    ZigBeeFrame{
        id: message_decrypted_layout
        text: "解密数据："
        show_tooltip: message_style.show_tooltip
        data_list: message_style.decrypted_text_list
        visible: model.decrypted_text ? true : false
        is_recieved: model.recieved
        component_width: message_style.component_width
        font_size: message_style.font_size
    }

    Rectangle{
        Layout.alignment: Qt.AlignHCenter
        implicitWidth: parent.width - 20
        implicitHeight: 1
        color: RibbonTheme.dark_mode ? "#525252" : "#D4D4D4"
        visible: note.visible && message_raw_layout.visible
        Behavior on color {
            ColorAnimation {
                duration: 60
                easing.type: Easing.OutSine
            }
        }
    }

    RibbonText {
        id: note
        Layout.alignment: Qt.AlignHCenter
        font.pixelSize: message_style.font_size
        color: RibbonTheme.dark_mode ? "white" : !model.recieved ? "white" : "black"
        text: model.note_text ? model.note_text : ""
        visible: model.note_text ? true : false
        Layout.preferredWidth: message_raw_layout.visible ? implicitWidth < (message_raw_layout.width - (message_raw_layout.padding * 2)) ?
                                   implicitWidth : (message_raw_layout.width - (message_raw_layout.padding * 2)) : implicitWidth < component_width ?
                                                                    implicitWidth : component_width
        wrapMode: RibbonText.Wrap
    }
}
