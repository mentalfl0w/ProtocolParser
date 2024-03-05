#include "zigbeeparser.h"
#include <QMutex>
#include <QThread>
#include <QRandomGenerator>

ZigBeeParser::ZigBeeParser(QObject *parent)
    : QObject{parent}
{
    _protocol = zigbee_protocol::Protocol::getInstance();
    _thread = new QThread();
    _resolver = ZigBeeDataResolver::instance();
    _resolver->moveToThread(_thread);
    _config = Config::instance();
    _bus = EventsBus::instance();
    _event.type("zigbee_parser");
    _bus->reg_event(&_event);
    connect(&_event,&Event::triggered,this,&ZigBeeParser::message_parser);
    connect(this, &ZigBeeParser::message_recved, _resolver, &ZigBeeDataResolver::message_parser);
    connect(_resolver, &ZigBeeDataResolver::data_send, this, &ZigBeeParser::bus_data_send);
    connect(_thread, &QThread::finished, this, [=](){
        _thread->deleteLater();
        _resolver->deleteLater();
    });
    _thread->start();
}

ZigBeeParser::~ZigBeeParser()
{
    _bus->unreg_event(&_event);
    disconnect(&_event,&Event::triggered,this,&ZigBeeParser::message_parser);
    if(_thread->isRunning())
    {
        _thread->quit();
    }
}

ZigBeeParser* ZigBeeParser::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    static ZigBeeParser *singleton = nullptr;
    if (!singleton) {
        singleton = new ZigBeeParser();
    }
    return singleton;
}

void ZigBeeParser::message_parser(QJsonObject message)
{
    emit message_recved(message, _self_addr);
}

void ZigBeeParser::bus_data_send(QString type, QJsonObject data)
{
    _bus->push_data(type, data);
}
