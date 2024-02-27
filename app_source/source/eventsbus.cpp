#include "eventsbus.h"
#include <QMutex>

EventsBus::EventsBus(QObject *parent)
    : QObject{parent}
{

}

EventsBus* EventsBus::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    static EventsBus *singleton = nullptr;
    if (!singleton) {
        singleton = new EventsBus();
    }
    return singleton;
}

void EventsBus::reg_event(Event* event)
{
    _events.append(event);
}

void EventsBus::unreg_event(Event* event)
{
    _events.removeOne(event);
}

void EventsBus::push_data(QString type, QJsonObject data)
{

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
