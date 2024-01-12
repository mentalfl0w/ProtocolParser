import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import RibbonUI

Item {
    clip: true
    property var header_items:[]
    property var header_items_width:[]
    property int row_spacing: 1
    property double margins: 30
    property int has_selected: 0
    property alias data_model: list_model
    property alias current_index: list_table.currentIndex
    property double max_height: 250
    ListModel{
        id: list_model
    }

    implicitHeight: (list_table.implicitHeight + list_table.anchors.margins * 2) > implicitWidth ?
                        implicitWidth : (list_table.implicitHeight + list_table.anchors.margins * 2)
    implicitWidth: getWidth()

    ListView{
        id: list_table
        anchors{
            top:parent.top
            horizontalCenter: parent.horizontalCenter
            margins: margins
        }
        implicitHeight: contentHeight > max_height ? max_height : contentHeight
        implicitWidth: parent.width - anchors.margins * 2
        clip: true
        model: list_model
        header: Item{
            implicitHeight: header_row.implicitHeight + 6
            implicitWidth: header_row.implicitWidth
            RowLayout{
                id:header_row
                spacing: row_spacing
                Repeater{
                    model: header_items
                    RibbonRectangle{
                        implicitHeight: head.implicitHeight + 10
                        implicitWidth: header_items_width[index]
                        topLeftRadius: index == 0 ? 5 : 0
                        bottomLeftRadius: index == 0 ? 5 : 0
                        topRightRadius: index == (header_items.length - 1) ? 5 : 0
                        bottomRightRadius: index == (header_items.length - 1) ? 5 : 0
                        color: RibbonTheme.dark_mode ? "#5E5C5B" : "#CBCAC9"
                        RibbonText{
                            id:head
                            anchors.centerIn: parent
                            width: implicitWidth > parent.width - 10 ? parent.width - 10 : implicitWidth
                            text: modelData
                            wrapMode: RibbonText.WordWrap
                            view_only: true
                        }
                    }
                }
            }
        }

        delegate: Item{
            implicitHeight: row_bg.implicitHeight + 6
            implicitWidth: row_bg.implicitWidth
            Rectangle{
                id: row_bg
                implicitHeight: row.implicitHeight
                implicitWidth: row.implicitWidth
                radius: 5
                color: mouse.containsMouse || row_bg.selected ? "#506BBD" : "transparent"
                property bool selected: false
                property bool is_focused: list_table.currentIndex === index
                onIs_focusedChanged: {
                    if (!is_focused && selected)
                        row_bg.selected = !row_bg.selected
                }
                onSelectedChanged: has_selected += row_bg.selected ? 1 : -1
                RowLayout{
                    id: row
                    spacing: row_spacing
                    Repeater{
                        model: getRowValues(index)
                        RibbonRectangle{
                            implicitHeight: text.implicitHeight + 10
                            implicitWidth: modelData.width
                            color: "transparent"
                            RibbonText{
                                id:text
                                anchors.centerIn: parent
                                width: implicitWidth > parent.width - 10 ? parent.width - 10 : implicitWidth
                                view_only: true
                                text: modelData.data
                                wrapMode: RibbonText.WordWrap
                                color: RibbonTheme.dark_mode ? mouse.containsMouse ? 'black' : 'white' : mouse.containsMouse ? 'white':'black'
                            }
                        }
                    }
                }
                MouseArea{
                    id: mouse
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked:
                    {
                        list_table.currentIndex = index
                        row_bg.selected = !row_bg.selected
                    }
                }
            }
        }

        add: Transition {
            NumberAnimation { properties: "x,y"; from: list_table.height; duration: 1000 }
        }
    }

    function getRowValues(index) {
        var obj = list_model.get(index)
        if(!obj)
            return
        var data = []

        for(let i=0;i<header_items.length;i++){
            data.push({data:obj[header_items[i]],width:header_items_width[i]})
        }
        return data;
    }

    function getWidth(){
        let width = 0;
        for (let i=0;i<header_items_width.length;i++)
        {
            width += header_items_width[i]
        }
        width += list_table.anchors.margins * 2
        width += (header_items_width.length - 1) * row_spacing
        return width
    }
}
