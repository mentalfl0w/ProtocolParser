import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import RibbonUI
import ProtocolParser

Item {
    id:obj
    property var header_items: ["序号","事件"]
    property var header_items_width: [100,400]
    property var data_source: []
    implicitHeight: list_layout.height
    implicitWidth: list_layout.width

    onData_sourceChanged: {
        list.data_model.clear()
        list.data_model.append(data_source)
    }

    Component.onCompleted: {
        var data = []
        for (var i = 0; i < EventsBus.event_history.length; i++)
        {
            data.push({"序号":i+1,"事件":JSON.stringify(EventsBus.event_history)})
        }
        data_source = data
    }

    Connections{
        target: EventsBus
        function onEvent_historyChanged(){
            var data = []
            for (var i = 0; i < EventsBus.event_history.length; i++)
            {
                data.push({"序号":i+1,"事件":JSON.stringify(EventsBus.event_history)})
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
            text: "事件列表"
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
                icon_source: RibbonIcons.HistoryDismiss
                text: qsTr("清空历史")
                enabled: EventsBus.event_history.length
                onClicked: {
                    EventsBus.event_history = []
                }
            }
        }
    }
}
