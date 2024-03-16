#include "tools.h"
#include <QMutex>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDirIterator>
#include <QTextCodec>

Tools::Tools(QObject *parent)
    : QObject{parent}
{
    baseDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation) + + "/ProtocolParser/");
}

Tools::~Tools()
{
    QDirIterator it(baseDir(), QDirIterator::Subdirectories);
    while (it.hasNext())
        QFile::remove(it.next());
}

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
    if (path.startsWith("qrc:/")) {
        path.replace(0, 3, "");
    }
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

void Tools::writeDirtoTempDir(QString path)
{
    if (path.startsWith("qrc:/")) {
        path.replace(0, 3, "");
    }
    QDir tmpdir(baseDir());
    QDirIterator it(path, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString tmpfile;
        tmpfile = it.next();
        if (QFileInfo(tmpfile).isFile()) {
            QString original_name = tmpfile;
            if(tmpfile.endsWith("js_packaged"))
                tmpfile.replace("js_packaged","js");
            tmpfile.replace(path,"");
            QFileInfo file = QFileInfo(tmpdir.absolutePath() + '/' + tmpfile);
            file.dir().mkpath("."); // create full path if necessary
            QFile::remove(file.absoluteFilePath()); // remove previous file to make sure we have the latest version
            QFile::copy(original_name, file.absoluteFilePath());
        }
    }
}

void Tools::writeFiletoDir(QString content, QString path, QString name, QByteArray tcodec)
{
    QFile file(path+'/'+name);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "File open fail: " + file.fileName();
        return;
    }
    QTextCodec *codec = QTextCodec::codecForName(tcodec);
    file.write(codec->fromUnicode(GetCorrectUnicode(content)));
    file.close();
}

QString Tools::GetCorrectUnicode(const QByteArray &ba)
{
    QTextCodec::ConverterState state;
    QTextCodec *codec = QTextCodec::codecForName("GB18030");
    QString text = codec->toUnicode( ba.constData(), ba.size(), &state);
    if (state.invalidChars > 0)
    {
        text = QTextCodec::codecForName( "UTF-8" )->toUnicode(ba);
        return text;
    }
    else
    {
        text = ba;
        return text;
    }
}

QString Tools::GetCorrectUnicode(const QString &ba)
{
    return GetCorrectUnicode(QByteArray(ba.toStdString().data()));
}
