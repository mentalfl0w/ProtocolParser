#ifndef EVENTBUSRESOLVER_H
#define EVENTBUSRESOLVER_H

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

class EventsBusResolver : public QObject
{
    Q_OBJECT
public:
    static EventsBusResolver* instance();
    QList<QJsonObject>& get_history(){return _event_history;};
public slots:
    void push_data(QString type, QJsonObject data);
    void reg_event(Event* event);
    void unreg_event(Event* event);
private:
    explicit EventsBusResolver(QObject *parent = nullptr);
    Q_DISABLE_COPY_MOVE(EventsBusResolver);
    QList<Event*> _events;
    QList<QJsonObject> _event_history;
};

#endif // EVENTBUSRESOLVER_H
