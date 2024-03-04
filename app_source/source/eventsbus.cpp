#include "eventsbus.h"
#include <QMutex>

bool EventsBus::is_destroy;

EventsBus::EventsBus(QObject *parent)
    : QObject{parent}
{
    is_destroy = false;
    _thread = new QThread();
    _resolver = EventsBusResolver::instance();
    _resolver->moveToThread(_thread);
    connect(this, &EventsBus::event_reg, _resolver, &EventsBusResolver::reg_event);
    connect(this, &EventsBus::event_unreg, _resolver, &EventsBusResolver::unreg_event);
    connect(this, &EventsBus::event_data_push, _resolver, &EventsBusResolver::push_data);
    connect(_thread, &QThread::finished, this, [=](){
        _thread->deleteLater();
        _resolver->deleteLater();
    });
    _thread->start();
}

EventsBus::~EventsBus()
{
    is_destroy = true;
   if(_thread->isRunning())
    {
        _thread->quit();

    }
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
    if (is_destroy)
        return;
    emit event_reg(event);
}

void EventsBus::unreg_event(Event* event)
{
    if (is_destroy)
        return;
    emit event_unreg(event);
}

void EventsBus::push_data(QString type, QJsonObject data)
{
    if (is_destroy)
        return;
    emit event_data_push(type,data);
}

