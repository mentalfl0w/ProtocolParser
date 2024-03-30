#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqmlextensionplugin.h>
#include <QTranslator>
#include <QQmlContext>
#include <QtWebView>
#include "ribbonui.h"

#ifdef RIBBONUI_BUILD_STATIC_LIB
Q_IMPORT_QML_PLUGIN(RibbonUIPlugin)
#endif
int main(int argc, char *argv[])
{
    RibbonUI::init();
    QtWebView::initialize();
    FramelessHelper::Quick::initialize();
    QGuiApplication app(argc, argv);
    QTranslator translator;
    bool result = translator.load(QLocale::system(), u""_qs, u""_qs, u":/translations"_qs);
    if (result)
        app.installTranslator(&translator);
    QQmlApplicationEngine engine;
    RibbonUI::registerTypes(&engine);
    QList<int> verl = {PROTOCOLPARSER_VERSION};
    QString version = QString::number(verl[0])+'.'+QString::number(verl[1])+'.'+QString::number(verl[2]);
    engine.rootContext()->setContextProperty("PPAPP_Version",version);
    const QUrl url(u"qrc:/qt/qml/ProtocolParser/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
