import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Window
import RibbonUI
import ProtocolParser
import org.wangwenx190.FramelessHelper
import "components"

RibbonWindow {
    id:root
    width: 1200
    minimumWidth: 1200
    height: 800
    minimumHeight: 800
    title: qsTr("Protocol Parser") + ` V${PPAPP_Version}`

    TabBar{
        id: tab_bar
        center_view: center_view
    }

    CenterView{
        id: center_view
        z:-2
        anchors{
            top: parent.top
            bottom: parent.bottom
        }
        width:parent.width
        tab_bar: tab_bar
        bottom_bar: bottom_bar
    }

    RibbonBottomBar{
        id: bottom_bar
        anchors{
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }

    Component.onCompleted: {
        RibbonTheme.modern_style=true
    }

    RibbonPopup{
        id:window_popup
        onClosed: window_popup.contentItem.destroy()
    }

    function show_popup(content_url, argument)
    {
        let component = Qt.createComponent(content_url,window_popup)

        if (component.status === Component.Error) {
            console.log(qsTr("Error loading component:"), component.errorString());
            return
        }
        else
        {
            if (typeof(argument)!='undefined')
                window_popup.contentItem = component.createObject(window_popup,argument)
            else
                window_popup.contentItem = component.createObject(window_popup)
        }
        window_popup.open()
    }

    function close_popup()
    {
        window_popup.close()
        window_popup.contentItem.destroy()
    }
}
