#include "serialportmanager.h"
#include <QSerialPortInfo>
#include <QMutex>
#include "zigbeeparser.h"

SerialPortManager::SerialPortManager(QObject *parent)
    : QObject{parent}
{
    _resolver = SerialDataResolver::instance();
    _thread = new QThread();
    _resolver->moveToThread(_thread);
    _bus = EventsBus::instance();
    ZigBeeParser::instance(); // 让ZigBeeParser在QML引擎中实例化
    _event.type("serial_port");
    _bus->reg_event(&_event);
    opened(false);
    handledBymanager(true);
    recv_hex(false);
    send_hex(false);
    send_type(SendType::Blank);
    connect(&_serial_port_scanner, &QTimer::timeout, this, [=](){
        available_ports();
    });
    connect(&_event, &Event::triggered, this, [=](QJsonObject data){
        write(data["text"].toString());
    });
    connect(_resolver,&SerialDataResolver::recved, this, [=](const QByteArray &data){
        emit recved(data);
    });
    connect(_resolver,&SerialDataResolver::recved_zframe, this, &SerialPortManager::zigbee_callback);
    connect(this, &SerialPortManager::recv_hexChanged, this, [=](){
        _resolver->recv_hex(recv_hex());
    });
    connect(this, &SerialPortManager::handledBymanagerChanged, this, [=](){
        _resolver->handled_by_manager(handledBymanager());
    });
    connect(_resolver,&SerialDataResolver::serial_openedChanged, this, [=](){
        opened(_resolver->serial_opened());
    });
    connect(this, &SerialPortManager::serial_close, _resolver, &SerialDataResolver::close);
    connect(this, &SerialPortManager::serial_open, _resolver, &SerialDataResolver::open);
    connect(this, &SerialPortManager::serial_write, _resolver, &SerialDataResolver::write_data);
    connect(_thread, &QThread::finished, this, [=](){
        _thread->deleteLater();
        _resolver->deleteLater();
    });
    _serial_port_scanner.setInterval(1000);
    _serial_port_scanner.start();
    _thread->start();
}

SerialPortManager::~SerialPortManager()
{
    close();
    _bus->unreg_event(&_event);
    if(_thread->isRunning())
    {
        _thread->quit();
    }
}

SerialPortManager* SerialPortManager::instance(){
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    static SerialPortManager *singleton = nullptr;
    if (!singleton) {
        singleton = new SerialPortManager();
    }
    return singleton;
}

QList<QString> SerialPortManager::available_ports(){
    QList<QString> newPortStringList;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        newPortStringList += info.portName();
    if (_available_ports_count != newPortStringList.count())
    {
        available_ports_count(newPortStringList.count());
        emit available_portsChanged(newPortStringList);
    }
    return newPortStringList;
}

void SerialPortManager::open(QString port_name, QString baudrate, QString databits,
                             QString parity, QString stopbits)
{
    emit serial_open(port_name,baudrate,databits,parity,stopbits);
}

void SerialPortManager::close()
{
    emit serial_close();
}

void SerialPortManager::write(QString data)
{
    emit serial_write(data,static_cast<SerialDataResolver::SendType>(send_type()),send_hex());
}

void SerialPortManager::zigbee_callback(zigbee_protocol::ZigbeeFrame zframe)
{
    QJsonObject object;
    QByteArray zf = QByteArray((char *)zframe.data(),zframe.size());
    QByteArray hex = zf.toHex(' ');
    QByteArray data = hex.toUpper();
    object.insert("data_hex", QJsonValue(QString(data)));
    object.insert("type","zigbee_raw_data");
    _bus->push_data("zigbee_parser", object);
}
