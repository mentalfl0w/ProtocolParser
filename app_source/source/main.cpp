#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtQml/qqmlextensionplugin.h>
#include <FramelessHelper/Quick/framelessquickmodule.h>
#include <FramelessHelper/Core/private/framelessconfig_p.h>
#include <QTranslator>
#include <QQmlContext>

FRAMELESSHELPER_USE_NAMESPACE
#ifdef RIBBONUI_BUILD_STATIC_LIB
Q_IMPORT_QML_PLUGIN(RibbonUIPlugin)
#endif
int main(int argc, char *argv[])
{
    qputenv("QT_QUICK_CONTROLS_STYLE","Basic");
    FramelessHelper::Quick::initialize();
    QGuiApplication app(argc, argv);

#ifdef Q_OS_WIN
    FramelessConfig::instance()->set(Global::Option::UseCrossPlatformQtImplementation);
    //FramelessConfig::instance()->set(Global::Option::ForceHideWindowFrameBorder);
    //FramelessConfig::instance()->set(Global::Option::ForceNonNativeBackgroundBlur);
#else
    FramelessConfig::instance()->set(Global::Option::EnableBlurBehindWindow);
#endif
    FramelessConfig::instance()->set(Global::Option::CenterWindowBeforeShow);
    QTranslator translator;
    bool result = translator.load(QLocale::system(), u""_qs, u""_qs, u":/translations"_qs);
    if (result)
        app.installTranslator(&translator);
    QQmlApplicationEngine engine;
    FramelessHelper::Quick::registerTypes(&engine);
    QList<int> verl = {PROTOCOLPARSER_VERSION};
    QString version = QString::number(verl[0])+'.'+QString::number(verl[1])+'.'+QString::number(verl[2]);
    engine.rootContext()->setContextProperty("PPAPP_Version",version);
#ifdef RIBBONUI_BUILD_STATIC_LIB
    engine.addImportPath("qrc:/");
#endif
    const QUrl url(u"qrc:/qt/qml/ProtocolParser/Main.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
