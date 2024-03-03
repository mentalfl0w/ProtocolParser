#ifndef EVENTSBUS_H
#define EVENTSBUS_H

#include <QObject>
#include <QQmlEngine>
#include <QJsonObject>
#include "../3rdparty/RibbonUI/lib_source/definitions.h"

class Event : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_NAMED_ELEMENT(Event)
    Q_PROPERTY_RW(QByteArray,type)
public:
    explicit Event(QObject *parent = nullptr){};
signals:
    void triggered(QJsonObject data);
};

class EventsBus : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(EventsBus)
    Q_PROPERTY_RW(QList<QJsonObject>, event_history)
public:
    static EventsBus* instance();
    static EventsBus* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine){return instance();}
    Q_INVOKABLE void push_data(QString type, QJsonObject data);
    Q_INVOKABLE void reg_event(Event* event);
    Q_INVOKABLE void unreg_event(Event* event);
private:
    explicit EventsBus(QObject *parent = nullptr);
    Q_DISABLE_COPY_MOVE(EventsBus);
    QList<Event*> _events;
};

#endif // EVENTSBUS_H
