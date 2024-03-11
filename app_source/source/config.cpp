#include "config.h"
#include <QtCore/QtCore>
#include <QDebug>

Config::Config(QString qstrfilename)
    : QObject(nullptr)
{
    if (qstrfilename.isEmpty())
    {
        m_qstrFileName = QCoreApplication::applicationDirPath() + "/config.ini";
    }
    else
    {
        m_qstrFileName = qstrfilename;
    }

    m_psetting = new QSettings(m_qstrFileName, QSettings::IniFormat);
}

Config::~Config()
{
    delete m_psetting;
    m_psetting = nullptr;
}

Config* Config::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    static Config *singleton = nullptr;
    if (!singleton) {
        singleton = new Config();
    }
    return singleton;
}

void Config::set(QString qstrnodename,QString qstrkeyname,QVariant qvarvalue)
{
    QMutexLocker locker(&_mutex);
    m_psetting->setValue(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname), qvarvalue);
}

void Config::setArray(QString qstrnodename,QString qstrkeyname,QVariant qvarvalue)
{
    QMutexLocker locker(&_mutex);
    m_psetting->beginWriteArray(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname));
    QList<QVariant> list = qvarvalue.toList();
    for (int i = 0; i< list.length(); i++)
    {
        m_psetting->setArrayIndex(i);
        m_psetting->setValue(qstrkeyname, list.at(i));
    }
    m_psetting->endArray();
}

QVariant Config::get(QString qstrnodename,QString qstrkeyname)
{
    QMutexLocker locker(&_mutex);
    QVariant qvar = m_psetting->value(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname));
    return qvar;
}

QVariant Config::getArray(QString qstrnodename,QString qstrkeyname)
{
    QMutexLocker locker(&_mutex);
    QList<QVariant> list;
    int size = m_psetting->beginReadArray(QString("/%1/%2").arg(qstrnodename).arg(qstrkeyname));
    for (int i = 0; i< size; i++)
    {
        m_psetting->setArrayIndex(i);
        list.append(m_psetting->value(qstrkeyname));
    }
    m_psetting->endArray();
    return list;
}

void Config::Clear()
{
    QMutexLocker locker(&_mutex);
    m_psetting->clear();
}
