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
    property string file_name: ""
    property string base_url: ""
    property string current_path: ""
    property string resource_dir: ""
    property int page_height: 0
    property alias can_goback: viewer.canGoBack
    property alias can_goforward: viewer.canGoForward

    onFile_nameChanged: reload()

    WebView{
        id: viewer
        anchors{
            top: parent.top
            left: parent.left
            margins: 30
        }
        width: parent.width
        height: parent.height
        settings.allowFileAccess: true
        settings.localStorageEnabled: true
        settings.javaScriptEnabled: true
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
                    pre_height = 0
                    load(request.url.toString())
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
        load(file_name)
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
            load(current_path)
        });
    }

    function load(file_path)
    {
        if(!file_path)
        {
            viewer.url = "about:blank"
            return
        }
        current_path = file_path
        let text = Tools.readAll(file_path)
        var script = `
            var md;
            let base_url = '${base_url}';
            var defaults = {
              html: true,
              xhtmlOut: true,
              breaks: false,
              linkify: true,
              typographer: true,
            };
            mermaid.initialize({
                startOnLoad:true,
                darkMode:${RibbonTheme.dark_mode},
                theme: "${RibbonTheme.dark_mode ? 'dark' : 'neutral'}",
                htmlLabels:true
            });
            const mermaidChart = (code) => {
                console.log(code)
                try {
                    return \`<div class=\"mermaid\">\${code}</div>\`
                } catch ({ str, hash }) {
                    return \`<pre>\${str}</pre>\`
                }
            }
            defaults.highlight = function (str, lang) {
                var esc = md.utils.escapeHtml;
                if (lang && hljs.getLanguage(lang)) {
                    try {
                        return '<pre class="hljs"><code>' +
                         hljs.highlight(lang, str, true).value +
                         '</code></pre>';
                    } catch (__) {}
                }else if(lang == 'mermaid'){
                    return mermaidChart(str)
                }else{
                    return '<pre class="hljs"><code>' + esc(str) + '</code></pre>';
                }
            };
            md = window.markdownit(defaults);
            md.use(markdownitDeflist);
            md.use(markdownitEmoji);
            md.use(markdownitSub);
            md.use(markdownitSup);
            md.use(markdownitAbbr);
            md.use(markdownitContainer);
            md.use(markdownitFootnote);
            md.use(markdownitIns);
            md.use(markdownitMark);

            let result = md.render($('.markdown-source').html());
            if (base_url.match(/^qrc?:\\/.+/g))
                result = result.replace(/href="(\\.\\/)?(.*?).md"/g, 'href="${base_url}/$2.md"');
            result = result.replace(/src=\\"(\\.\\/)?/g, \`src=\"${resource_dir}\`);
            $('.markdown-body').html(result);
            $('.markdown-source').remove();
        `
        // var md = markdownit({
        //                         html: true,
        //                         linkify: true,
        //                         typographer: true
        //                     });
        // md.use(markdownitDeflist)
        // md.use(markdownitEmoji)
        // md.use(markdownitSub)
        // md.use(markdownitSup)
        // md.use(markdownitAbbr)
        // md.use(markdownitContainer)
        // md.use(markdownitFootnote)
        // md.use(markdownitIns)
        // md.use(markdownitMark)
        // var result = md.render(text);
        // if (base_url.match(/^qrc?:\/.+/))
        //     result = result.replace(new RegExp("href=\"(\.\/)?(.*?).md", "gi"), `href="${base_url}/$2.md"`);
        // result = result.replace(new RegExp("src=\"(./)?", "gi"), `src="`+resource_dir);
        let parts = file_path.split('/')
        let full_file_name = parts[parts.length-1] === '/' ? parts[parts.length-2] : parts[parts.length-1]
        let file_name = full_file_name.split('.')
        let html_name = `${file_name[0]}.html`
        let prefix = `file:${Qt.platform.os === 'windows' ? '///' : '//'}` + Tools.baseDir
        let html = `<!DOCTYPE html><html data-theme=${RibbonTheme.dark_mode ? 'dark' : 'light'} xmlns="http://www.w3.org/1999/xhtml" xml:lang="zh-CN" lang="zh-CN">` +
                                                               `<head><meta charset="UTF-8", name="viewport" content="width=device-width, initial-scale=1">` +
                                                               `<title>${full_file_name}</title>` +
                                                               `<link rel="stylesheet" href=${prefix}resources/theme.css></style>` +
                                                               `<link rel="stylesheet" href=${prefix}resources/github-${RibbonTheme.dark_mode ? 'dark' : 'light'}.css></style>` +
                                                               `<script src=${prefix}jquery.js></script>` +
                                                               `<script src=${prefix}markdown-it.js></script>`+
                                                               `<script src=${prefix}markdown-it-deflist.js></script>` +
                                                               `<script src=${prefix}markdown-it-emoji.js></script>` +
                                                               `<script src=${prefix}markdown-it-sub.js></script>` +
                                                               `<script src=${prefix}markdown-it-abbr.js></script>` +
                                                               `<script src=${prefix}markdown-it-container.js></script>` +
                                                               `<script src=${prefix}markdown-it-footnote.js></script>` +
                                                               `<script src=${prefix}markdown-it-ins.js></script>` +
                                                               `<script src=${prefix}markdown-it-mark.js></script>` +
                                                               `<script src=${prefix}markdown-it-sup.js></script>` +
                                                               `<script src=${prefix}highlight.js></script>` +
                                                               `<script src=${prefix}mermaid.js></script>`+
                                                               "</head>" + "<body>" +
                                                               '<article class="markdown-body">' + '</article>' +
                                                               '<pre class="markdown-source">' + text + '</pre>' +
                                                               `<script type="module">${script}</script>` +
                                                               "</body></html>"
        Tools.writeFiletoDir(html,Tools.baseDir,html_name)
        viewer.url = (prefix + html_name)
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
