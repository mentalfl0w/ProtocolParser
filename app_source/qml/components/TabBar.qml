import QtQuick
import QtQuick.Layouts
import RibbonUI
import ProtocolParser

RibbonTabBar {
    property bool send_hex: serial_send_hex_checkbox.checked
    property bool show_tooltip: message_parse_checkbox.checked
    property bool auto_scroll_to_bottom: view_scroll_bottom_checkbox.checked
    property var center_view

    RibbonTabPage{
        id: serial_page
        title: qsTr("串口设置")
        RibbonTabGroup{
            text: qsTr("参数配置")
            width: serial_layout.width + 30
            RowLayout{
                id: serial_layout
                anchors.centerIn: parent
                height: parent.height
                spacing: 20
                ColumnLayout{
                    Layout.fillHeight: true
                    spacing: 20
                    RowLayout{
                        Layout.preferredWidth: 340 + spacing
                        RibbonText{
                            text: qsTr("端   口:")
                            view_only: true
                        }
                        RibbonComboBox{
                            id: serial_port_combo
                            Layout.fillWidth: true
                            model: ListModel {
                                id: serial_model
                            }
                            icon_source: RibbonIcons.SerialPort
                            currentIndex: 0
                            enabled: !SerialPortManager.opened
                            Connections{
                                target: SerialPortManager
                                function onAvailable_portsChanged(ports){
                                    serial_model.clear()
                                    for (let i=0;i<SerialPortManager.available_ports_count;i++)
                                        serial_model.append({text:ports[i]})
                                }
                            }
                        }
                    }
                    RowLayout{
                        Layout.preferredWidth: 340 + spacing
                        spacing: 10
                        RowLayout{
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            RibbonText{
                                text: qsTr("波特率:")
                                view_only: true
                            }
                            RibbonComboBox{
                                id: serial_baudrate_combo
                                Layout.fillWidth: true
                                model: ListModel {
                                    ListElement { text: "2400" }
                                    ListElement { text: "4800" }
                                    ListElement { text: "9600" }
                                    ListElement { text: "19200" }
                                    ListElement { text: "38400" }
                                    ListElement { text: "57600" }
                                    ListElement { text: "115200" }
                                }
                                enabled: !SerialPortManager.opened
                                currentIndex: 6
                                icon_source: RibbonIcons.NumberSymbol
                            }
                        }
                        RowLayout{
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            RibbonText{
                                text: qsTr("数据位:")
                                view_only: true
                            }
                            RibbonComboBox{
                                id: serial_databits_combo
                                Layout.fillWidth: true
                                model: ListModel {
                                    ListElement { text: "5" }
                                    ListElement { text: "6" }
                                    ListElement { text: "7" }
                                    ListElement { text: "8" }
                                }
                                enabled: !SerialPortManager.opened
                                currentIndex: 3
                                icon_source: RibbonIcons.DataWhisker
                            }
                            RibbonText{
                                text: qsTr("位")
                                view_only: true
                            }
                        }
                    }
                }
                ColumnLayout{
                    Layout.fillHeight: true
                    Layout.preferredWidth: 140 + spacing
                    spacing: 20
                    RowLayout{
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        RibbonText{
                            text: qsTr("校验码:")
                            view_only: true
                        }
                        RibbonComboBox{
                            id: serial_parity_combo
                            Layout.fillWidth: true
                            model: ListModel {
                                ListElement { text: qsTr("NONE") }
                                ListElement { text: qsTr("EVEN") }
                                ListElement { text: qsTr("ODD") }
                            }
                            enabled: !SerialPortManager.opened
                            icon_source: RibbonIcons.DataWhisker
                        }
                    }
                    RowLayout{
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        RibbonText{
                            text: qsTr("停止位:")
                            view_only: true
                        }
                        RibbonComboBox{
                            id: serial_stopbits_combo
                            Layout.fillWidth: true
                            model: ListModel {
                                ListElement { text: "1" }
                                ListElement { text: "1.5" }
                                ListElement { text: "2" }
                            }
                            enabled: !SerialPortManager.opened
                            icon_source: RibbonIcons.DataWhisker
                        }
                    }
                }
                RibbonPushButton{
                    id: serial_open_btn
                    Layout.alignment: Qt.AlignVCenter
                    icon_source: RibbonIcons.Open
                    show_tooltip: false
                    text: SerialPortManager.opened ? qsTr("关闭串口") : qsTr("打开串口")
                    onClicked: !SerialPortManager.opened ? SerialPortManager.open(serial_port_combo.currentText,
                                                      serial_baudrate_combo.currentText,
                                                      serial_databits_combo.currentText,
                                                      serial_parity_combo.currentText,
                                                      serial_stopbits_combo.currentText) :
                                                          SerialPortManager.close()
                }
            }
        }
        RibbonTabGroup{
            text: qsTr("串口消息设置")
            width: serial_message_layout.width + 30
            RowLayout{
                id: serial_message_layout
                anchors.centerIn: parent
                height: parent.height - 20
                ColumnLayout{
                    RibbonCheckBox{
                        id: message_parse_checkbox
                        text: qsTr("显示消息解析")
                        checked: true
                    }
                    RibbonCheckBox{
                        id: view_scroll_bottom_checkbox
                        text: qsTr("自动定位最新消息")
                        checked: true
                    }
                    RibbonButton{
                        Layout.fillWidth: true
                        icon_source: RibbonIcons.DismissCircle
                        text: qsTr("清除旧消息")
                        show_tooltip: false
                        onClicked: {
                            if (center_view.currentIndex === 0)
                                center_view.zigbee_dataview.clear_all_message()
                            else
                                center_view.serial_assistant.clear_all_message()
                        }
                    }
                }
            }
        }
        RibbonTabGroup{
            text: qsTr("串口助手设置")
            width: serial_assistant_layout.width + 30
            ColumnLayout{
                id: serial_assistant_layout
                anchors.centerIn: parent
                height: parent.height - 20
                spacing: 10
                RibbonCheckBox{
                    id: serial_recv_hex_checkbox
                    text: qsTr("十六进制接收")
                    checked: SerialPortManager.recv_hex
                    onCheckedChanged: SerialPortManager.recv_hex = checked
                }
                RibbonCheckBox{
                    id: serial_send_hex_checkbox
                    text: qsTr("十六进制发送")
                    checked: SerialPortManager.send_hex
                    onCheckedChanged: SerialPortManager.send_hex = checked
                }
            }
        }
        RibbonTabGroup{
            text: qsTr("串口数据流向")
            width: current_serial_indicator.contentWidth + 30
            RibbonText{
                anchors.centerIn: parent
                id: current_serial_indicator
                text: qsTr("传输至：") + (center_view.currentIndex === 0 ? qsTr("ZigBee") : qsTr("串口助手"))
            }
        }
    }
    RibbonTabPage{
        id: zigbee_protocol_page
        title: qsTr("ZigBee协议")
        RibbonTabGroup{
            width: device_list_layout.width + 30
            text: qsTr("设备列表")
            ColumnLayout{
                id: device_list_layout
                anchors.centerIn: parent
                height: parent.height
                spacing: 10
                RibbonButton{
                    Layout.fillWidth: true
                    icon_source: RibbonIcons.List
                    text: qsTr("等待队列")
                    show_tooltip: false
                    onClicked: {
                        show_popup("components/DeviceList.qml",{list_type:0})
                    }
                }
                RibbonButton{
                    Layout.fillWidth: true
                    icon_source: RibbonIcons.List
                    text: qsTr("认证列表")
                    show_tooltip: false
                    onClicked: {
                        show_popup("components/DeviceList.qml",{list_type:1})
                    }
                }
                RibbonButton{
                    Layout.fillWidth: true
                    icon_source: RibbonIcons.List
                    text: qsTr("黑名单列表")
                    show_tooltip: false
                    onClicked: {
                        show_popup("components/DeviceList.qml",{list_type:2})
                    }
                }
            }
        }
        RibbonTabGroup{
            width: key_manage_layout.width + 30
            text: qsTr("密钥管理")
            ColumnLayout{
                id: key_manage_layout
                anchors.centerIn: parent
                height: parent.height
                spacing: 0
                RowLayout{
                    spacing: 5
                    RibbonText{
                        text: qsTr("修改密钥：")
                    }
                    RibbonLineEdit{
                        id: key_manage_edit
                        placeholderText: qsTr("HEX形式16字节密钥")
                        onCommit: key_manage_save_btn.clicked()
                    }
                    RibbonButton{
                        id: key_manage_save_btn
                        icon_source: RibbonIcons.Save
                        text: qsTr("保存")
                        show_tooltip: false
                        onClicked: {
                            var keys = []
                            keys = ZigBeeParser.pre_hmac_verify_key
                            keys.push(ZigBeeParser.hmac_verify_key)
                            ZigBeeParser.hmac_verify_key = key_manage_edit.text
                            ZigBeeParser.pre_hmac_verify_key = keys
                            key_manage_edit.clear()
                        }
                    }
                }
                RibbonButton{
                    Layout.fillWidth: true
                    icon_source: RibbonIcons.List
                    text: qsTr("历史密钥列表")
                    show_tooltip: false
                    onClicked: {
                        show_popup("components/KeysList.qml")
                    }
                }
            }
        }
        RibbonTabGroup{
            text: qsTr("调试选项")
            width: zigbee_debug_layout.width + 30
            RowLayout{
                id: zigbee_debug_layout
                anchors.centerIn: parent
                height: parent.height
                spacing: 10
                ColumnLayout{
                    spacing: 10
                    RibbonButton{
                        Layout.fillWidth: true
                        icon_source: RibbonIcons.Mail
                        text: qsTr("生成接收数据")
                        tip_text: qsTr("在接收数据流中生成一条测试接收数据")
                        onClicked: {
                            EventsBus.push_data("zigbee_parser",{
                                                    type: "demo_recv_data",
                                                    data_hex:zigbee_debug_layout.generate_value()
                                                })
                        }
                    }
                    RibbonButton{
                        Layout.fillWidth: true
                        icon_source: RibbonIcons.Mail
                        text: qsTr("生成验证数据")
                        tip_text: qsTr("在验证信息流中生成一组测试验证数据")
                        onClicked: {
                            EventsBus.push_data("zigbee_parser",{
                                                    type: "demo_verify_request"
                                                })
                        }
                    }
                }
                ColumnLayout{
                    spacing: 10
                    RibbonButton{
                        Layout.fillWidth: true
                        icon_source: RibbonIcons.Mail
                        text: qsTr("生成更换密钥数据")
                        tip_text: qsTr("在验证信息流中生成一组测试更换密钥数据，需要有至少一条过期密钥")
                        enabled: ZigBeeParser.pre_hmac_verify_key.length
                        onClicked: {
                            EventsBus.push_data("zigbee_parser",{
                                                    type: "demo_verify_key_update"
                                                })
                        }
                    }
                    RibbonButton{
                        Layout.fillWidth: true
                        icon_source: RibbonIcons.History
                        text: qsTr("事件历史信息")
                        tip_text: qsTr("查看所有事件历史信息（包含调试信息）")
                        onClicked: {
                            show_popup("components/EventsHistoryList.qml")
                        }
                    }
                }
                function generate_value(){
                    function get_rand_byte()
                    {
                        return Math.floor(Math.random()*16).toString(16).toUpperCase()
                    }

                    let repeat_count = 16
                    let t = `FF 29 ${get_rand_byte()}${get_rand_byte()} 83 56 56 `+
                        `AA AD 56 56 52 48 23 32 01 00 17 00 00 00 AA AA 01 10 00 FF FF `
                    for (let j=0;j<repeat_count;j++){
                        t+=` ${get_rand_byte()}${get_rand_byte()}`
                    }
                    t+=" FE"
                    return t
                }
            }
        }
    }
    RibbonTabPage{
        id: other_page
        title: qsTr("其他")
        RibbonTabGroup{
            width: theme_layout.width + 30
            text: qsTr("主题")
            RowLayout{
                id: theme_layout
                anchors.centerIn: parent
                height: parent.height
                spacing: 10
                RibbonComboBox{
                    id: theme_combobox
                    component_width: 120
                    model: ListModel {
                        id: model_theme
                        ListElement { text: qsTr("浅色") }
                        ListElement { text: qsTr("深色") }
                        ListElement { text: qsTr("跟随系统") }
                    }
                    icon_source: RibbonIcons.DarkTheme
                    Component.onCompleted: {
                        let str = (RibbonTheme.theme_mode === RibbonThemeType.System ? qsTr("跟随系统") : RibbonTheme.theme_mode === RibbonThemeType.Light ? qsTr("浅色") : qsTr("深色"))
                        currentIndex = find(str)
                    }
                    onActivated: {
                        if (currentText === qsTr("跟随系统"))
                            RibbonTheme.theme_mode = RibbonThemeType.System
                        else if (currentText === qsTr("浅色"))
                            RibbonTheme.theme_mode = RibbonThemeType.Light
                        else
                            RibbonTheme.theme_mode = RibbonThemeType.Dark
                    }
                    Connections{
                        target: RibbonTheme
                        function onTheme_modeChanged(){
                            let str = (RibbonTheme.theme_mode === RibbonThemeType.System ? qsTr("跟随系统") : RibbonTheme.theme_mode === RibbonThemeType.Light ? qsTr("浅色") : qsTr("深色"))
                            theme_combobox.currentIndex = theme_combobox.find(str)
                        }
                    }
                }
            }
        }
    }
}
