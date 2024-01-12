import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import RibbonUI
import ProtocolParser

Item{
    id: obj
    property string title: list_type === 0 ? "等待列表" : list_type === 1 ? "认证列表" : "黑名单列表"
    property var header_items: ["序号","地址"]
    property var header_items_width: [100,200]
    property var data_source: []
    property int list_type: 0
    implicitHeight: list_layout.height
    implicitWidth: list_layout.width

    onData_sourceChanged: {
        list.data_model.clear()
        list.data_model.append(data_source)
    }

    Component.onCompleted: {
        var data = []
        switch(list_type){
        case 0:
        {
            for (var i = 0; i < ZigBeeParser.wait_queue.length; i++)
            {
                data.push({"序号":i+1,"地址":'0x'+ZigBeeParser.wait_queue[i].toString(16).toUpperCase()})
            }
            break
        }
        case 1:
        {
            for (let i = 0; i < ZigBeeParser.allow_list.length; i++)
            {
                data.push({"序号":i+1,"地址":'0x'+ZigBeeParser.allow_list[i].toString(16).toUpperCase()})
            }
            break
        }
        case 2:
        {
            for (let i = 0; i < ZigBeeParser.deny_list.length; i++)
            {
                data.push({"序号":i+1,"地址":'0x'+ZigBeeParser.deny_list[i].toString(16).toUpperCase()})
            }
            break
        }
        }
        data_source = data
    }

    Connections{
        target: ZigBeeParser
        function onWait_queueChanged(){
            if (list_type !== 0)
                return
            let data = []
            for (let i = 0; i < ZigBeeParser.wait_queue.length; i++)
            {
                data.push({"序号":i+1,"地址":'0x'+ZigBeeParser.wait_queue[i].toString(16).toUpperCase()})
            }
            data_source = data
        }

        function onAllow_listChanged(){
            if (list_type !== 1)
                return
            let data = []
            for (let i = 0; i < ZigBeeParser.allow_list.length; i++)
            {
                data.push({"序号":i+1,"地址":'0x'+ZigBeeParser.allow_list[i].toString(16).toUpperCase()})
            }
            data_source = data
        }

        function onDeny_listChanged(){
            if (list_type !== 2)
                return
            let data = []
            for (let i = 0; i < ZigBeeParser.deny_list.length; i++)
            {
                data.push({"序号":i+1,"地址":'0x'+ZigBeeParser.deny_list[i].toString(16).toUpperCase()})
            }
            data_source = data
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
        id:list_layout
        anchors{
            bottom: parent.bottom
            horizontalCenter: parent.horizontalCenter
        }

        RibbonText{
            id: title_text
            Layout.topMargin: 30
            Layout.alignment: Qt.AlignHCenter
            text: title
            view_only: true
            font.pixelSize: 15
        }

        ListTable {
            id: list
            header_items: obj.header_items
            header_items_width: obj.header_items_width
            margins: 0
            Layout.topMargin: 10
            Layout.leftMargin: 30
            Layout.rightMargin: Layout.leftMargin
            Layout.bottomMargin: Layout.leftMargin
            Layout.preferredHeight: implicitHeight > 250 ? 250 : implicitHeight
        }

        RowLayout{
            spacing: 30
            Layout.alignment: Qt.AlignHCenter
            Layout.bottomMargin: 30
            RibbonButton{
                show_tooltip: false
                icon_source: RibbonIcons.Add
                text: list_type !== 2 ? qsTr("加入黑名单") : qsTr("加入认证列表")
                enabled: list.has_selected
                onClicked: {
                    let deny = ZigBeeParser.deny_list
                    var data = []
                    switch(list_type){
                        case 0:
                        {
                            deny.push(ZigBeeParser.wait_queue[list.current_index])
                            ZigBeeParser.deny_list = deny
                            for (var i = 0; i < ZigBeeParser.wait_queue.length; i++)
                            {
                                if (i === list.current_index)
                                    continue
                                data.push(ZigBeeParser.wait_queue[i])
                            }
                            ZigBeeParser.wait_queue = data
                            break
                        }
                        case 1:
                        {
                            deny.push(ZigBeeParser.allow_list[list.current_index])
                            ZigBeeParser.deny_list = deny
                            for (let i = 0; i < ZigBeeParser.allow_list.length; i++)
                            {
                                if (i === list.current_index)
                                    continue
                                data.push(ZigBeeParser.allow_list[i])
                            }
                            ZigBeeParser.allow_list = data
                            break
                        }
                        case 2:
                        {
                            let allow = ZigBeeParser.allow_list
                            allow.push(ZigBeeParser.deny_list[list.current_index])
                            ZigBeeParser.allow_list = allow
                            let data = []
                            for (let i = 0; i < ZigBeeParser.deny_list.length; i++)
                            {
                                if (i === list.current_index)
                                    continue
                                data.push(ZigBeeParser.deny_list[i])
                            }
                            ZigBeeParser.deny_list = data
                            break
                        }
                    }
                }
            }
            RibbonButton{
                show_tooltip: false
                icon_source: list_type === 0 ? RibbonIcons.Add : RibbonIcons.Delete
                text: list_type === 0 ? qsTr("加入认证列表") : list_type === 1 ?
                                            qsTr("移出认证列表") : qsTr("移出黑名单列表")
                enabled: list.has_selected
                onClicked: {
                    switch(list_type){
                    case 0:
                    {
                        let allow = ZigBeeParser.allow_list
                        allow.push(ZigBeeParser.wait_queue[list.current_index])
                        ZigBeeParser.allow_list = allow
                        var data = []
                        for (var i = 0; i < ZigBeeParser.wait_queue.length; i++)
                        {
                            if (i === list.current_index)
                                continue
                            data.push(ZigBeeParser.wait_queue[i])
                        }
                        ZigBeeParser.wait_queue = data
                        break
                    }
                    case 1:
                    {
                        let data = []
                        for (let i = 0; i < ZigBeeParser.allow_list.length; i++)
                        {
                            if (i === list.current_index)
                                continue
                            data.push(ZigBeeParser.allow_list[i])
                        }
                        ZigBeeParser.allow_list = data
                        break
                    }
                    case 2:
                    {
                        let data = []
                        for (let i = 0; i < ZigBeeParser.deny_list.length; i++)
                        {
                            if (i === list.current_index)
                                continue
                            data.push(ZigBeeParser.deny_list[i])
                        }
                        ZigBeeParser.deny_list = data
                        break
                    }
                    }
                }
            }
        }
    }
}
