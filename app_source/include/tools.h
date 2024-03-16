#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QQmlEngine>
#include <QQuickTextDocument>
#include "../3rdparty/RibbonUI/lib_source/definitions.h"

class Tools : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(Tools)
    Q_PROPERTY_RW(QString, baseDir)
public:
    static Tools* instance();
    static Tools* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine){return instance();}
    Q_INVOKABLE void setDefaultStyleSheet(QQuickTextDocument *qd, QString path);
    Q_INVOKABLE QString readAll(QString path);
    Q_INVOKABLE void setBaseUrl(QQuickTextDocument *qd, QUrl url);
    Q_INVOKABLE QString fileSuffix(const QString &filePath);
    Q_INVOKABLE QString fileDir(const QString &filePath);
    Q_INVOKABLE void writeDirtoTempDir(QString path);
    Q_INVOKABLE void writeFiletoDir(QString content, QString path, QString name);

private:
    explicit Tools(QObject *parent = nullptr);
    ~Tools();
    Q_DISABLE_COPY_MOVE(Tools)

signals:
};

#endif // TOOLS_H
