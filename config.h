#ifndef CONFIG_H
#define CONFIG_H

#include <QQmlEngine>
#include <QVariant>
#include <QSettings>

class Config : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_NAMED_ELEMENT(Config)
public:
    static Config* instance();
    static Config* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine){return instance();}
    Q_INVOKABLE void Set(QString qstrnodename,QString qstrkeyname,QVariant qvarvalue);
    Q_INVOKABLE void SetArray(QString qstrnodename,QString qstrkeyname,QVariant qvarvalue);
    Q_INVOKABLE QVariant Get(QString qstrnodename,QString qstrkeyname);
    Q_INVOKABLE QVariant GetArray(QString qstrnodename,QString qstrkeyname);
    void Clear();
private:
    Config(QString qstrfilename = "");
    ~Config(void);
    QString m_qstrFileName;
    QSettings *m_psetting=nullptr;
};

#endif // CONFIG_H

