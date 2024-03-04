#ifndef EVENTSBUS_H
#define EVENTSBUS_H

#include <QObject>
#include <QQmlEngine>
#include <QJsonObject>
#include <QThread>
#include "eventbusresolver.h"

class EventsBus : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(EventsBus)
    Q_PROPERTY(QList<QJsonObject> event_history READ event_history WRITE setEvent_history NOTIFY event_historyChanged FINAL)
public:
    static EventsBus* instance();
    static EventsBus* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine){return instance();}
    QList<QJsonObject> event_history(){if(!_resolver)_resolver = EventsBusResolver::instance();return _resolver->get_history();};
    void setEvent_history(QList<QJsonObject> event_history){if(!_resolver)_resolver = EventsBusResolver::instance();QList<QJsonObject> &a = _resolver->get_history(); a = event_history; emit event_historyChanged();};
    Q_INVOKABLE void push_data(QString type, QJsonObject data);
    Q_INVOKABLE void reg_event(Event* event);
    Q_INVOKABLE void unreg_event(Event* event);
private:
    explicit EventsBus(QObject *parent = nullptr);
    ~EventsBus();
    Q_DISABLE_COPY_MOVE(EventsBus);
    QThread* _thread = nullptr;
    EventsBusResolver* _resolver = nullptr;
    static bool is_destroy;
signals:
    void event_reg(Event* event);
    void event_unreg(Event* event);
    void event_data_push(QString type, QJsonObject data);
    void event_historyChanged();
};

#endif // EVENTSBUS_H
