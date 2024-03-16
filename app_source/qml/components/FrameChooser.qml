import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import RibbonUI
import ProtocolParser

Item {
    clip: true
    id: root
    implicitWidth: layout.width + margins*2
    implicitHeight: layout.height + margins*2
    property double margins: 30
    property int object_width: 30

    ListModel{
        id: frame_model
        ListElement{
            name:"温度"
            type:"float"
        }
        ListElement{
            name:"湿度"
            type:"float"
        }
        ListElement{
            name:"气体浓度"
            type:"float"
        }
        ListElement{
            name:"火焰指数"
            type:"float"
        }
    }

    Component.onCompleted: {
        let result = Config.getArray('Protocol', 'data_frame_name')
        if (result.length)
        {
            frame_model.clear()
            var name_array=[],type_array=[]
            name_array = Config.getArray('Protocol', 'data_frame_name')
            type_array = Config.getArray('Protocol', 'data_frame_type')
            for (var i=0;i<name_array.length;i++)
            {
                frame_model.append({
                                       name:name_array[i],
                                       type:type_array[i]
                                   })
            }
        }
        else
        {
            let name_array=[],type_array=[]
            for (let i=0;i<frame_model.count;i++)
            {
                name_array[i] = frame_model.get(i).name
                type_array[i] = frame_model.get(i).type
            }
            Config.setArray('Protocol', 'data_frame_name', name_array)
            Config.setArray('Protocol', 'data_frame_type', type_array)
        }
    }

    RibbonButton{
        anchors{
            top:parent.top
            margins: 4
            right:parent.right
        }
        show_bg: false
        show_hovered_bg: false
        icon_source: RibbonIcons.Dismiss
        onClicked: window_popup.close()
    }

    ColumnLayout{
        id:layout
        anchors{
            top:parent.top
            horizontalCenter: parent.horizontalCenter
            margins: margins
        }
        RibbonText{
            Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
            width: implicitWidth > parent.width ? parent.width : implicitWidth
            text: qsTr("自定义传感器数据结构")
            font.pixelSize: 20
            wrapMode: RibbonText.WordWrap
            view_only: true
        }
        RibbonText{
            Layout.topMargin: margins/2
            Layout.alignment: Qt.AlignTop | Qt.AlignLeft
            width: implicitWidth > parent.width ? parent.width : implicitWidth
            text: qsTr("传感器数据帧结构组成:")
            wrapMode: RibbonText.WordWrap
            view_only: true
        }
        Rectangle{
            id: frame_bg
            radius: 5
            Layout.preferredHeight: object_width + frame.anchors.leftMargin*2
            Layout.preferredWidth: object_width * 16 + frame.anchors.leftMargin*2
            color: RibbonTheme.dark_mode ? '#2A2C31' : '#F2F4F7'
            ListView{
                id: frame
                property int current_size: 0
                property bool is_full: current_size === 16
                property int has_selected: 0
                clip: true
                model: frame_model
                orientation: ListView.Horizontal
                implicitHeight: object_width
                implicitWidth: contentWidth
                anchors{
                    leftMargin: 2
                    left: parent.left
                    verticalCenter: parent.verticalCenter
                }
                delegate:RibbonRectangle{
                    id: bg
                    topLeftRadius: index === 0 ? 5 : 0
                    bottomLeftRadius: topLeftRadius
                    topRightRadius: frame.current_size === 16 && (index + 1) === frame.count ? 5 : 0
                    bottomRightRadius: topRightRadius
                    implicitHeight: object_width
                    implicitWidth: object_width
                    color: bg.selected || mouse.containsMouse ? RibbonTheme.dark_mode ? "#8AAAEB" : "#506BBD" : RibbonTheme.dark_mode ? '#3A4045' : "white"
                    property bool selected: false
                    property bool is_focused: frame.currentIndex === index
                    onIs_focusedChanged: {
                        if (!is_focused && selected)
                            bg.selected = !bg.selected
                    }
                    onSelectedChanged: frame.has_selected += bg.selected ? 1 : -1
                    property string type: model.type
                    property int data_size
                    onTypeChanged: {
                        data_size = cal_size(model.type)
                        implicitWidth = object_width * data_size
                    }
                    RibbonText{
                        anchors.centerIn: parent
                        width: implicitWidth > parent.width ? parent.width : implicitWidth
                        text: model.name
                        wrapMode: RibbonText.WordWrap
                        view_only: true
                        color:{
                            if(!RibbonTheme.dark_mode)
                            {
                                if(mouse.containsMouse&&!bg.selected)
                                    return 'white'
                                else if(mouse.containsMouse&&bg.selected)
                                    return 'black'
                                else if(!mouse.containsMouse&&bg.selected)
                                    return 'white'
                                else
                                    return 'black'
                            }
                            else
                            {
                                if(mouse.containsMouse&&!bg.selected)
                                    return 'white'
                                else if(mouse.containsMouse&&bg.selected)
                                    return 'black'
                                else if(!mouse.containsMouse&&bg.selected)
                                    return 'white'
                                else
                                    return 'white'
                            }
                        }
                    }
                    Rectangle{
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        width: 1
                        height: parent.height - frame.anchors.leftMargin * 2
                        visible: index + 1 !== frame.count
                        color: RibbonTheme.dark_mode ? "#525252" : "#D4D4D4"
                        Behavior on color {
                            ColorAnimation {
                                duration: 60
                                easing.type: Easing.OutSine
                            }
                        }
                    }
                    RibbonToolTip{
                        text: qsTr('类型：')+ `${bg.type}, ` + qsTr('长度：') + `${bg.data_size}B`
                        visible: text && mouse.containsMouse
                    }

                    MouseArea{
                        id: mouse
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked:
                        {
                            frame.currentIndex = index
                            bg.selected = !bg.selected
                        }
                    }
                    ListView.onRemove: cal_size(type,true)
                    function cal_size(type,is_remove)
                    {
                        let size = frame.judge_size(type);
                        if(!is_remove)
                            frame.current_size += size
                        else
                            frame.current_size -= size
                        return size
                    }
                }
                function judge_size(type)
                {
                    let size = 0
                    switch(type)
                    {
                    case "float":
                        size = 4
                        break
                    case "double":
                        size = 8
                        break
                    case "int64_t":
                        size = 8
                        break
                    case "int32_t":
                        size = 4
                        break
                    case "int16_t":
                        size = 2
                        break
                    case "int8_t":
                        size = 1
                        break
                    case "uint64_t":
                        size = 8
                        break
                    case "uint32_t":
                        size = 4
                        break
                    case "uint16_t":
                        size = 2
                        break
                    case "uint8_t":
                        size = 1
                        break
                    }
                    return size
                }
            }
        }
        RowLayout{
            Layout.preferredWidth: parent.width
            Layout.topMargin: margins/2
            Layout.alignment: Qt.AlignHCenter
            spacing: margins/3
            RibbonText{
                text: qsTr("类型：")
                view_only: true
            }
            RibbonComboBox{
                id: type_combo
                Layout.preferredWidth: 100
                model: ListModel {
                    ListElement { text: "float" }
                    ListElement { text: "double" }
                    ListElement { text: "int64_t" }
                    ListElement { text: "int32_t" }
                    ListElement { text: "int16_t" }
                    ListElement { text: "int8_t" }
                    ListElement { text: "uint64_t" }
                    ListElement { text: "uint32_t" }
                    ListElement { text: "uint16_t" }
                    ListElement { text: "uint8_t" }
                }
                enabled: frame.current_size !==16
            }
            RibbonText{
                text: qsTr("名称：")
                view_only: true
            }
            RibbonLineEdit{
                id: name_edit
                Layout.preferredWidth: 120
                placeholderText: qsTr("请输入数据名称")
                onCommit: save_btn.clicked()
                enabled: frame.current_size !==16
            }
            RibbonButton{
                id: add_btn
                icon_source: RibbonIcons.Add
                text: qsTr("添加")
                show_tooltip: false
                enabled: {
                    if (frame.judge_size(type_combo.currentText)+frame.current_size>16)
                        return false
                    else
                        return true
                }
                onClicked: {
                    frame_model.append({
                                           type:type_combo.currentText,
                                           name:name_edit.text
                                       })
                }
            }
            RibbonButton{
                id: delete_btn
                icon_source: RibbonIcons.Delete
                text: qsTr("删除")
                show_tooltip: false
                enabled: frame.has_selected !== 0
                onClicked: {
                    frame_model.remove(frame.currentIndex)
                }
            }
        }
        RowLayout{
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: margins/2
            spacing: margins
            RibbonButton{
                id: save_btn
                Layout.preferredWidth: 150
                Layout.alignment: Qt.AlignHCenter
                icon_source: RibbonIcons.Save
                text: qsTr("保存")
                show_tooltip: false
                onClicked: {
                    if (!type_combo.enabled && !name_edit.enabled && frame.current_size > 0)
                    {
                        let name_array=[],type_array=[]
                        for (let i=0;i<frame_model.count;i++)
                        {
                            name_array[i] = frame_model.get(i).name
                            type_array[i] = frame_model.get(i).type
                        }
                        Config.setArray('Protocol', 'data_frame_name', name_array)
                        Config.setArray('Protocol', 'data_frame_type', type_array)
                    }
                    else
                    {
                        name_edit.text = qsTr("请检查数据长度")
                    }
                }
            }
            RibbonButton{
                id: reset_btn
                Layout.preferredWidth: 150
                icon_source: RibbonIcons.ArrowReset
                text: qsTr("还原默认值")
                show_tooltip: false
                onClicked: {
                    frame_model.clear()
                    frame_model.append([{
                                           name:"温度",
                                           type:"float"
                                       },{
                                           name:"湿度",
                                           type:"float"
                                       },{
                                           name:"气体浓度",
                                           type:"float"
                                       },{
                                           name:"火焰指数",
                                           type:"float"
                                       }])
                }
            }
        }
    }
}
