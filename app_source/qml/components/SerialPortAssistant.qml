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

    onAuto_scroll_to_bottomChanged: {
        serial_view.auto_scroll_to_bottom = auto_scroll_to_bottom
        if(auto_scroll_to_bottom)
            serial_view.scroll_to_bottom()
    }

    RibbonMessageListView{
        id: serial_view
        anchors.fill: parent
        top_padding: serial_title_bar.height + control.top_padding + (!RibbonTheme.modern_style ? 10 : 0)
        bottom_padding: message_sender.height + control.bottom_padding
        auto_scroll_to_bottom: control.auto_scroll_to_bottom
        delegate: ZigBeeMessage{
            sender_text: `${model.time} ${model.recieved ? '收' : '发'}`
            show_tooltip: control.show_tooltip
            component_width: serial_view.width / 2
        }
        Connections{
            target: SerialPortManager
            function onRecved(data)
            {
                serial_view.message_model.append({
                                                     time: Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss.zzz"),
                                                     note_text: data.toString(),
                                                     recieved: true,
                                                 })
            }
        }
    }
    RibbonRectangle{
        id: serial_title_bar
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: control.top_padding + (!RibbonTheme.modern_style ? 10 : 0)
        implicitWidth: parent.width - 20
        implicitHeight: serial_title.contentHeight + 10
        radius: 5
        color: RibbonTheme.dark_mode ? Qt.rgba(0, 0, 0, 0.5) : Qt.rgba(255, 255, 255, 0.5)
        RibbonText {
            id: serial_title
            anchors.centerIn: parent
            text: "串口助手"
            color: RibbonTheme.dark_mode ? "white" : "black"
            font.pixelSize: 15
            view_only: true
        }
    }
    RibbonRectangle{
        id: message_sender
        property double padding: 10
        property bool folded: false
        anchors{
            bottom: parent.bottom
            bottomMargin: serial_view.bottom_padding - height
            horizontalCenter: parent.horizontalCenter
        }
        color: RibbonTheme.dark_mode ? Qt.rgba(0, 0, 0, 0.5) : Qt.rgba(255, 255, 255, 0.5)
        radius: 10
        bottomLeftRadius: 0
        bottomRightRadius: bottomLeftRadius
        width: serial_view.width
        height: message_sender_btn.height + fold_btn.height + padding*2
        states: [
            State{
                name:"fold"
                when: message_sender.folded
                PropertyChanges {
                    target: message_sender
                    height:fold_btn.height
                }
            },
            State{
                name:"unfold"
                when: !message_sender.folded
                PropertyChanges {
                    target: message_sender
                    height: (message_sender_btn.height + fold_btn.height + padding*2)
                }
            }
        ]
        transitions: [
            Transition {
                from: "fold"
                to:"unfold"
                NumberAnimation {
                    properties: "height"
                    duration: 167
                    easing.type: Easing.OutSine
                }
            },
            Transition {
                from: "unfold"
                to:"fold"
                NumberAnimation {
                    properties: "height"
                    duration: 167
                    easing.type: Easing.OutSine
                }
            }
        ]

        RibbonButton{
            id: fold_btn
            anchors{
                right: parent.right
                top: parent.top
            }
            icon_source: RibbonIcons.ChevronUp
            Behavior on rotation {
                NumberAnimation{
                    duration: 100
                    easing.type: Easing.OutSine
                }
            }
            rotation: message_sender.folded ? 0 : 180
            onClicked: message_sender.folded = !message_sender.folded
            show_bg: false
            show_hovered_bg: false
            tip_text: message_sender.folded ? qsTr("显示信息发送栏") : qsTr("隐藏信息发送栏")
            text_color: dark_mode ? "white" : "black"
        }
        RowLayout{
            anchors{
                top: fold_btn.bottom
                left: parent.left
                right: parent.right
            }
            RowLayout{
                Layout.preferredWidth: 200
                Layout.fillHeight: true
                Layout.margins: 10
                RibbonText{
                    text: "发送结束符:"
                    view_only: true
                }
                RibbonComboBox{
                    id: serial_send_type_combo
                    Layout.fillHeight: true
                    model: ListModel {
                        ListElement { text: "无" }
                        ListElement { text: "换行" }
                        ListElement { text: "回车" }
                        ListElement { text: "换行和回车" }
                    }
                    enabled: SerialPortManager.opened
                    icon_source: RibbonIcons.DataWhisker
                }
            }
            RibbonTextEdit{
                Layout.fillWidth: true
                Layout.preferredHeight: max_height
                Layout.alignment: Qt.AlignHCenter
                id:message_sender_textbox
                max_height: message_sender_btn.height
                icon_source: RibbonIcons.TextField
                placeholderText:  SerialPortManager.send_hex ?  "请输入需要发送的信息（以16进制的形式输入, 空格分隔）：" : "请输入需要发送的信息："
                enabled: SerialPortManager.opened
                onCommit: {
                    if (text)
                        message_sender_btn.clicked()
                }
            }
            RibbonPushButton{
                Layout.alignment: Qt.AlignVCenter
                id:message_sender_btn
                text: "发送"
                show_tooltip: false
                icon_source: RibbonIcons.Send
                onClicked: {
                    SerialPortManager.send_hex = tab_bar.send_hex
                    SerialPortManager.send_type = serial_send_type_combo.currentText === "无" ? SerialPortManager.Blank
                                                                                             : serial_send_type_combo.currentText === "回车" ? SerialPortManager.WithCarriageEnter
                                                                                                                                           : serial_send_type_combo.currentText === "换行" ? SerialPortManager.WithLineFeed
                                                                                                                                                                                         : SerialPortManager.WithCarriageEnterAndLineFeed
                    let data = input_validate(message_sender_textbox.text)
                    switch(data)
                    {
                    case 'empty':
                        message_sender_textbox.text = "待发送内容为空！"
                        return
                    case 'hex':
                        message_sender_textbox.text = "请严格按照16进制输入！"
                        return
                    case 'case':
                        message_sender_textbox.text = "请使用全大写或全小写16进制输入！"
                        return
                    case 'blank':
                        message_sender_textbox.text = "首尾部不可出现空格！"
                        return
                    case 'separate':
                        message_sender_textbox.text = "字节间需用空格分隔！"
                        return
                    default:
                        break
                    }
                    SerialPortManager.write(data)
                    serial_view.message_model.append({
                                                         time: Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss.zzz"),
                                                         note_text: message_sender_textbox.text,
                                                         recieved: false,
                                                     })
                    message_sender_textbox.textedit.clear()
                }
                enabled: SerialPortManager.opened
                function input_validate(str)
                {
                    if (str.length === 0 && serial_send_type_combo.currentText === "无")
                        return 'empty';
                    if (!/^[0-9a-z ]+$/i.test(str) && SerialPortManager.send_hex) {
                        return 'hex';
                    }
                    if ((!(str === str.toUpperCase() || str === str.toLowerCase())) && SerialPortManager.send_hex) {
                        return 'case';
                    }
                    if ((str.startsWith(' ') || str.endsWith(' ')) && SerialPortManager.send_hex) {
                        return 'blank';
                    }
                    for (let i = 0; (i < str.length - 1) && SerialPortManager.send_hex; i += 3) {
                        if (str[i + 2] !== ' ' && i !== str.length - 2) {
                            return 'separate';
                        }
                    }
                    return str;
                }
            }
        }
    }

    function clear_all_message()
    {
        serial_view.message_model.clear()
    }
}
