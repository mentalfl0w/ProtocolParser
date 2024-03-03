#ifndef SERIALDATARESOLVER_H
#define SERIALDATARESOLVER_H

#include <QObject>
#include <QMutex>
#include <QSerialPort>
#include "DLLN3X.h"

class SerialDataResolver : public QObject
{
    Q_OBJECT
public:
    enum class SendType{
        Blank=0,
        WithCarriageEnter=1,
        WithLineFeed=2,
        WithCarriageEnterAndLineFeed=3
    };
    Q_ENUM(SendType)
    void open(QString port_name, QString baudrate, QString databits,
                       QString parity, QString stopbits);
    void close();
    bool handled_by_manager();
    void handled_by_manager(bool handled_by_manager);
    bool recv_hex();
    void recv_hex(bool recv_hex);
    bool serial_opened();
    static SerialDataResolver* instance();
public slots:
    void write_data(QString data, SendType type, bool send_hex);

private:
    explicit SerialDataResolver(QObject *parent = nullptr);
    ~SerialDataResolver();
    Q_DISABLE_COPY_MOVE(SerialDataResolver)
    void readHandler();
    void zigbee_callback(zigbee_protocol::ZigbeeFrame zframe);
    void serial_opened(bool serial_opened);
    zigbee_protocol::DLLN3X* _zigbee=nullptr;
    QSerialPort* _serial=nullptr;
    bool _quit = false, _handled_by_manager = false, _recv_hex = false,
        _serial_opened = false,_serial_want_open = false;

signals:
    void error(const QString &s);
    void recved(const QByteArray &data);
    void recved_zframe(zigbee_protocol::ZigbeeFrame zframe);
    void handled_by_managerChanged();
    void recv_hexChanged();
    void serial_openedChanged();
};

#endif // SERIALDATARESOLVER_H
