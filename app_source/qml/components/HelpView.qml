import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import RibbonUI
import ProtocolParser
import "."

Item {
    id:root
    implicitHeight: 500
    implicitWidth: 500
    property string title: qsTr("帮助")

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

    RibbonText{
        anchors{
            top:parent.top
            topMargin: 10
            horizontalCenter: parent.horizontalCenter
        }
        text: title
    }

    RibbonMarkDownViewer{
        id: viewer
        anchors.fill: parent
        text: Tools.readAll(':/qt/qml/ProtocolParser/documents/menu.md')
        base_url: 'qrc:/qt/qml/ProtocolParser/documents/'
    }

    RibbonButton{
        anchors{
            bottom: root.bottom
            bottomMargin: 10
            left: root.left
            leftMargin: 30
        }
        show_bg: false
        show_hovered_bg: false
        icon_source: RibbonIcons.ArrowReply
        text: qsTr('返回目录')
        onClicked: viewer.go_back()
        visible: viewer.can_goback
        show_tooltip: false
    }

    RibbonButton{
        anchors{
            bottom: root.bottom
            bottomMargin: 10
            right: root.right
            rightMargin: 30
        }
        show_bg: false
        show_hovered_bg: false
        icon_source: RibbonIcons.ArrowForward
        text: qsTr('返回帮助')
        onClicked: viewer.go_forward()
        visible: viewer.can_goforward
        show_tooltip: false
    }
}
