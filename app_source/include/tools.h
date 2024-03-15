#ifndef TOOLS_H
#define TOOLS_H

#include <QObject>
#include <QQmlEngine>
#include <QQuickTextDocument>

class Tools : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(Tools)
public:
    static Tools* instance();
    static Tools* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine){return instance();}
    Q_INVOKABLE void setDefaultStyleSheet(QQuickTextDocument *qd, QString path);
    Q_INVOKABLE QString readAll(QString path);
    Q_INVOKABLE void setBaseUrl(QQuickTextDocument *qd, QUrl url);
    Q_INVOKABLE QString fileSuffix(const QString &filePath);
    Q_INVOKABLE QString fileDir(const QString &filePath);

private:
    explicit Tools(QObject *parent = nullptr);
    Q_DISABLE_COPY_MOVE(Tools)

signals:
};

#endif // TOOLS_H
