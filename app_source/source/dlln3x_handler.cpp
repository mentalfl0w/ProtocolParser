#include "dlln3x_handler.h"

DLLN3X_Handler::DLLN3X_Handler(QObject *parent)
    : QObject{parent}
{
    _zigbee = zigbee_protocol::DLLN3X::instance();
    handle_serial(false);
    serial(nullptr);
    connect(this, &DLLN3X_Handler::handle_serialChanged, this, [=](){
        if (_handle_serial&&_serial!=nullptr)
        {
            _zigbee->init(_serial, _serial->baudRate());
            connect(_zigbee, &zigbee_protocol::DLLN3X::recved, this, &DLLN3X_Handler::zigbee_callback);
            connect(_serial, &QSerialPort::readyRead, _zigbee, &zigbee_protocol::DLLN3X::loop);
            connect(_serial, &QSerialPort::aboutToClose, this, &DLLN3X_Handler::close);
        }
        else
        {
            close();
        }
    });
}

void DLLN3X_Handler::zigbee_callback(zigbee_protocol::ZigbeeFrame zframe)
{
    QByteArray zf = QByteArray((char *)zframe.data(),zframe.size());
    QByteArray hex = zf.toHex(' ');
    QByteArray data = hex.toUpper();
    QByteArray num = QByteArray::number(zframe.getRemoteAddr(),16);
    QByteArray sender = num.toUpper();
    emit zigbee_recved(data, sender);
}

void DLLN3X_Handler::close()
{
    disconnect(_zigbee, &zigbee_protocol::DLLN3X::recved, this, &DLLN3X_Handler::zigbee_callback);
    disconnect(_serial, &QSerialPort::readyRead, _zigbee, &zigbee_protocol::DLLN3X::loop);
    disconnect(_serial, &QSerialPort::aboutToClose, this, &DLLN3X_Handler::close);
}

bool DLLN3X_Handler::send(QString data)
{
    if (_handle_serial)
    {
        QByteArray bdata;
        QStringList td = data.split(' ');
        for (auto item : td)
            bdata += QByteArray::fromHex(item.toLatin1());

        zigbee_protocol::ZigbeeFrame zf(bdata.data(),bdata.length());

        return _zigbee->send(zf);
    }
    else
        return false;
}
