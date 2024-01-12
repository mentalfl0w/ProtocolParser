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

    onAuto_scroll_to_bottomChanged: if(auto_scroll_to_bottom) serial_view.scroll_to_bottom()

    RibbonMessageListView{
        id: serial_view
        anchors.fill: parent
        top_padding: serial_title_bar.height + control.top_padding + (!RibbonTheme.modern_style ? 10 : 0)
        bottom_padding: message_sender.height + control.bottom_padding
        delegate: ZigBeeMessage{
            sender_text: `${Qt.formatDateTime(new Date(), "yyyy-MM-dd hh:mm:ss.zzz")} ${model.recieved ? '收' : '发'}`
            show_tooltip: control.show_tooltip
            component_width: serial_view.width / 2
        }
        view.onHeightChanged: {
            if (control.auto_scroll_to_bottom)
                scroll_to_bottom()
        }
        Connections{
            target: SerialPortManager
            function onReadyReadChanged(){
                if (control.handle_serial && SerialPortManager.readyRead)
                {
                    var d = SerialPortManager.read()
                    serial_view.message_model.append({
                                             note_text: d.toString(),
                                             recieved: true,
                                         })
                }

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
                    SerialPortManager.write(message_sender_textbox.text)
                    serial_view.message_model.append({
                                             note_text: message_sender_textbox.text,
                                             recieved: false,
                                         })
                    message_sender_textbox.textedit.clear()
                }

                enabled: SerialPortManager.opened
            }
        }
    }

    function clear_all_message()
    {
        serial_view.message_model.clear()
    }
}
