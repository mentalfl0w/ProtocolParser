import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import RibbonUI
import ProtocolParser
import org.wangwenx190.FramelessHelper

RibbonWindow {
    id:root
    width: 1200
    minimumWidth: 1200
    height: 800
    minimumHeight: 800
    title: qsTr("Protocol Parser") + ` V${PPAPP_Version}`

    title_bar.right_content:RowLayout{
        spacing: 1
        layoutDirection: Qt.RightToLeft
        RibbonButton{
            show_bg:false
            icon_source: RibbonIcons.QuestionCircle
            icon_source_filled: RibbonIcons_Filled.QuestionCircle
            tip_text: qsTr("帮助")
            hover_color: Qt.rgba(0,0,0, 0.3)
            pressed_color: Qt.rgba(0,0,0, 0.4)
            text_color: title_bar.title_text_color
            text_color_reverse: false
            onClicked: {
                Window.window.popup.show_content("qrc:/qt/qml/ProtocolParser/components/HelpView.qml")
            }
        }
    }

    CenterView{
        id: center_view
        anchors{
            top: parent.top
            bottom: parent.bottom
        }
        width:parent.width
        tab_bar: tab_bar
        bottom_bar: bottom_bar 
    }

    TabBar{
        id: tab_bar
        center_view: center_view
    }

    RibbonBottomBar{
        id: bottom_bar
        anchors{
            left: parent.left
            right: parent.right
            bottom: center_view.bottom
        }
    }

    Component.onCompleted: {
        RibbonTheme.modern_style=true
    }
}
