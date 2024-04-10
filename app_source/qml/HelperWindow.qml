import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import RibbonUI
import ProtocolParser
import "."

RibbonWindow {
    id: root
    height: 550
    width: 550
    title_bar.show_darkmode_btn: false
    title_bar.show_style_switch: false
    title: qsTr("帮助")
    Component.onCompleted: Tools.writeDirtoTempDir(":/qt/qml/ProtocolParser/")

    RibbonMarkDownViewer{
        id: viewer
        anchors.fill: parent
        anchors{
            topMargin: 10
            leftMargin: anchors.topMargin
            rightMargin: anchors.topMargin
            bottomMargin: viewer.can_goback || viewer.can_goforward ? 30 : 10
        }
        file_name: 'qrc:/qt/qml/ProtocolParser/README.md'
        base_url: 'qrc:/qt/qml/ProtocolParser/'
        resource_dir: Tools.baseDir
    }

    RibbonButton{
        anchors{
            topMargin: 5
            bottom: parent.bottom
            bottomMargin: 5
            left: parent.left
            leftMargin: viewer.anchors.leftMargin
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
            topMargin: 5
            bottom: parent.bottom
            bottomMargin: 5
            right: parent.right
            rightMargin: viewer.anchors.rightMargin
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
