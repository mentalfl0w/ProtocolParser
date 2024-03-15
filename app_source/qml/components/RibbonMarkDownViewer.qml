import QtQuick
import RibbonUI
import QtWebView
import ProtocolParser
import "qrc:/qt/qml/ProtocolParser/markdown-it.js" as MarkdownIt
import "qrc:/qt/qml/ProtocolParser/markdown-it-deflist.js" as MarkdownItDeflist
import "qrc:/qt/qml/ProtocolParser/markdown-it-emoji.js" as MarkdownItEmoji
import "qrc:/qt/qml/ProtocolParser/markdown-it-sub.js" as MarkdownItSub
import "qrc:/qt/qml/ProtocolParser/markdown-it-sup.js" as MarkdownItSup
import "qrc:/qt/qml/ProtocolParser/markdown-it-abbr.js" as MarkdownItAbbr
import "qrc:/qt/qml/ProtocolParser/markdown-it-container.js" as MarkdownItContainer
import "qrc:/qt/qml/ProtocolParser/markdown-it-footnote.js" as MarkdownItFootnote
import "qrc:/qt/qml/ProtocolParser/markdown-it-ins.js" as MarkdownItIns
import "qrc:/qt/qml/ProtocolParser/markdown-it-mark.js" as MarkdownItMark

Item {
    id:root
    property string text: ""
    property string base_url: ""
    property int page_height: 0
    property alias can_goback: viewer.canGoBack
    property alias can_goforward: viewer.canGoForward

    onTextChanged: reload()

    WebView{
        id: viewer
        anchors{
            top: parent.top
            left: parent.left
            margins: 30
        }
        width: parent.width
        height: parent.height
        property int pre_height: 0
        onLoadingChanged: function(request){
            if (request.status === WebView.LoadStartedStatus)
            {
                if(request.url.toString().match(/^http(s)?:\/\/.+/))
                {
                    viewer.stop()
                    Qt.openUrlExternally(request.url)
                }
                else if (request.url.toString().match(/^qrc?:\/.+/))
                {
                    console.log(request.url)
                    text = Tools.readAll(request.url.toString().replace("qrc:/",":/"))
                    pre_height = 0
                    load()
                }
            }
            else if (request.status === WebView.LoadSucceededStatus)
            {
                viewer.width = parent.width - (anchors.margins * 2)
                viewer.height = parent.height - (anchors.margins * 2)
                viewer.runJavaScript(`document.body.scrollTop = ${viewer.pre_height};`)
                get_height()
            }
            else if (request.status === WebView.LoadFailedStatus)
                console.error(request.errorString)
        }
    }


    Component.onCompleted: {
        load()
    }

    Connections{
        target: RibbonTheme
        function onDark_modeChanged(){
            reload()
        }
    }

    function get_height()
    {
        viewer.runJavaScript("document.body.scrollHeight", function(height) {
            page_height = height
        });
    }

    function set_current_height(height)
    {
        viewer.runJavaScript(`document.body.scrollTop = ${height};`)
        viewer.pre_height = height
    }

    function reload()
    {
        viewer.runJavaScript("document.body.scrollTop", function(height) {
            viewer.pre_height = height
            load()
        });
    }

    function load()
    {
        var style = Tools.readAll(":/qt/qml/ProtocolParser/resources/theme.css")
        var prism = Tools.readAll(`:/qt/qml/ProtocolParser/resources/prism-${RibbonTheme.dark_mode ? 'dark' : 'light'}.css`)
        var prismjs = Tools.readAll(":/qt/qml/ProtocolParser/prism.js")
        var ex = `
        html {
        height:100%;
        overflow:hidden;
        position:relative;
        }
        .markdown-body {
        box-sizing: border-box;
        min-width: 200px;
        max-width: 980px;
        margin: 0 auto;
        padding: 45px;
        height:100%;
        overflow:auto;
        position:relative;
        }

        @media (max-width: 767px) {
        .markdown-body {
        padding: 15px;
        }
        }`
        var md = markdownit({
                                html: true,
                                linkify: true,
                                typographer: true
                            });
        md.use(markdownitDeflist)
        md.use(markdownitEmoji)
        md.use(markdownitSub)
        md.use(markdownitSup)
        md.use(markdownitAbbr)
        md.use(markdownitContainer)
        md.use(markdownitFootnote)
        md.use(markdownitIns)
        md.use(markdownitMark)
        var result = md.render(text);
        result = result.replace(new RegExp("href=\"(./)?", "gi"), 'href="'+base_url);
        viewer.loadHtml(`<html data-theme=${RibbonTheme.dark_mode ? 'dark' : 'light'}>` + '<head><meta charset="UTF-8", name="viewport" content="width=device-width, initial-scale=1">' +
                                                                    "<style>" + ex + prism + style + "</style></head>" + "<body>" + '<script>' + prismjs + '</script>' +
                                                                    '<article class="markdown-body">' + result + '</article>' + "</body></html>")
    }

    function go_back()
    {
        viewer.goBack()
    }

    function go_forward()
    {
        viewer.goForward()
    }
}
