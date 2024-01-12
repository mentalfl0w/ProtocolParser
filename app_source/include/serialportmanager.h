#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QSerialPort>
#include "DLLN3X.h"
#include "../3rdparty/RibbonUI/lib_source/definitions.h"
#include "zigbeeparser.h"
#include "eventsbus.h"

class SerialPortManager : public QObject
{

    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(SerialPortManager)
    Q_PROPERTY(QList<QString> available_ports READ available_ports() NOTIFY available_portsChanged FINAL)

public:
    enum class SendType{
        Blank=0,
        WithCarriageEnter=1,
        WithLineFeed=2,
        WithCarriageEnterAndLineFeed=3
    };
    Q_ENUM(SendType)

    Q_PROPERTY_RW(int, available_ports_count)
    Q_PROPERTY_RW(bool, opened)
    Q_PROPERTY_RW(bool, closed)
    Q_PROPERTY_RW(bool, readyRead)
    Q_PROPERTY_RW(bool, handledBymanager)
    Q_PROPERTY_RW(bool, recv_hex)
    Q_PROPERTY_RW(bool, send_hex)
    Q_PROPERTY_RW(SendType, send_type)

public:
    static SerialPortManager* instance();
    static SerialPortManager* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine){return instance();}
    QList<QString> available_ports();
    Q_INVOKABLE bool open(QString port_name, QString baudrate, QString databits,
                          QString parity, QString stopbits);
    Q_INVOKABLE void close();
    Q_INVOKABLE QByteArray read();
    Q_INVOKABLE bool write(QString data);
    Q_INVOKABLE QSerialPort* get_serial();

signals:
    void available_portsChanged(QList<QString> ports);
private:
    explicit SerialPortManager(QObject *parent = nullptr);
    ~SerialPortManager();
    QTimer _serial_port_scanner;
    QSerialPort _serial_port;
    zigbee_protocol::DLLN3X* _zigbee=nullptr;
    void _ready_for_read();
    void zigbee_callback(zigbee_protocol::ZigbeeFrame zframe);
    Event _event;
    EventsBus* _bus = nullptr;
};

#endif // SERIALPORTMANAGER_H
