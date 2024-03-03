#ifndef SERIALPORTMANAGER_H
#define SERIALPORTMANAGER_H

#include <QObject>
#include <QQmlEngine>
#include <QTimer>
#include <QSerialPort>
#include <QThread>
#include "serialdataresolver.h"
#include "../3rdparty/RibbonUI/lib_source/definitions.h"
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
    Q_PROPERTY_RW(bool, readyRead)
    Q_PROPERTY_RW(bool, handledBymanager)
    Q_PROPERTY_RW(bool, recv_hex)
    Q_PROPERTY_RW(bool, send_hex)
    Q_PROPERTY_RW(SendType, send_type)

public:
    static SerialPortManager* instance();
    static SerialPortManager* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine){return instance();}
    QList<QString> available_ports();
    Q_INVOKABLE void open(QString port_name, QString baudrate, QString databits,
                          QString parity, QString stopbits);
    Q_INVOKABLE void close();
    Q_INVOKABLE void write(QString data);

signals:
    void available_portsChanged(QList<QString> ports);
    void recved(QString data);
    void serial_open(QString port_name, QString baudrate, QString databits,
                     QString parity, QString stopbits);
    void serial_close();
    void serial_write(QString data, SerialDataResolver::SendType type, bool send_hex);
private:
    explicit SerialPortManager(QObject *parent = nullptr);
    ~SerialPortManager();
    Q_DISABLE_COPY_MOVE(SerialPortManager)
    QTimer _serial_port_scanner;
    Event _event;
    EventsBus* _bus = nullptr;
    SerialDataResolver* _resolver = nullptr;
    QThread* _thread = nullptr;

private slots:
    void zigbee_callback(zigbee_protocol::ZigbeeFrame zframe);
};

#endif // SERIALPORTMANAGER_H
