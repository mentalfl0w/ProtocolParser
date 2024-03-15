#include "tools.h"
#include <QMutex>
#include <QFile>
#include <QFileInfo>

Tools::Tools(QObject *parent)
    : QObject{parent}
{}

Tools* Tools::instance(){
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    static Tools *singleton = nullptr;
    if (!singleton) {
        singleton = new Tools();
    }
    return singleton;
}

void Tools::setDefaultStyleSheet(QQuickTextDocument *qd, QString path)
{
    auto td = qd->textDocument();
    td->setDefaultStyleSheet(readAll(path));
}

QString Tools::readAll(QString path)
{
    QFile file(path);
    if(!file.exists()){
        qWarning()<<"File not exist!";
        return "";
    }
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text)){
        qWarning()<<"Cannot open the file!";
        return "";
    }
    auto content = file.readAll();
    return QString(content);
}

void Tools::setBaseUrl(QQuickTextDocument *qd, QUrl url)
{
    qd->textDocument()->setBaseUrl(url);
}

QString Tools::fileSuffix(const QString &filePath)
{
    QFileInfo info(filePath);
    return info.suffix();
}

QString Tools::fileDir(const QString &filePath)
{
    QFileInfo info(filePath);
    return info.absolutePath();
}
