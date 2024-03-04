#include "eventbusresolver.h"
#include <QMutex>

EventsBusResolver::EventsBusResolver(QObject *parent)
    : QObject{parent}
{

}

EventsBusResolver* EventsBusResolver::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    static EventsBusResolver *singleton = nullptr;
    if (!singleton) {
        singleton = new EventsBusResolver();
    }
    return singleton;
}

void EventsBusResolver::reg_event(Event* event)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    _events.append(event);
}

void EventsBusResolver::unreg_event(Event* event)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    _events.removeOne(event);
}

void EventsBusResolver::push_data(QString type, QJsonObject data)
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);
    if (_event_history.count() >= 100)
        _event_history.pop_front();
    if (data["time"].toString()=="")
        data.insert("time",QJsonValue(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz")));
    _event_history.append(data);
    for (auto event : _events) {
        if (event->type() == type)
            emit event->triggered(data);
    }
}
