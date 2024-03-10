#ifndef CONFIG_H
#define CONFIG_H

#include <QQmlEngine>
#include <QVariant>
#include <QSettings>

class Config : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(Config)
public:
    static Config* instance();
    static Config* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine){return instance();}
    Q_INVOKABLE void set(QString qstrnodename,QString qstrkeyname,QVariant qvarvalue);
    Q_INVOKABLE void setArray(QString qstrnodename,QString qstrkeyname,QVariant qvarvalue);
    Q_INVOKABLE QVariant get(QString qstrnodename,QString qstrkeyname);
    Q_INVOKABLE QVariant getArray(QString qstrnodename,QString qstrkeyname);
    void Clear();
private:
    explicit Config(QString qstrfilename = "");
    ~Config(void);
    Q_DISABLE_COPY_MOVE(Config)
    QString m_qstrFileName;
    QSettings *m_psetting=nullptr;
};

#endif // CONFIG_H

