import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import RibbonUI
import ProtocolParser
import "."

Item {
    id:root
    implicitHeight: 550
    implicitWidth: 550
    property string title: qsTr("帮助")
    Component.onCompleted: Tools.writeDirtoTempDir(":/qt/qml/ProtocolParser/")

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
        anchors.margins: 35
        file_name: 'qrc:/qt/qml/ProtocolParser/README.md'
        base_url: 'qrc:/qt/qml/ProtocolParser/'
        resource_dir: Tools.baseDir
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
