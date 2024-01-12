#ifndef DLLN3X_HANDLER_H
#define DLLN3X_HANDLER_H

#include <QObject>
#include <QQmlEngine>
#include <QSerialPort>
#include "DLLN3X.h"
#include "3rdparty/RibbonUI/lib_source/definitions.h"

class DLLN3X_Handler : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(DLLN3XHandler)
    Q_PROPERTY_RW(bool, handle_serial);
    Q_PROPERTY_RW(QSerialPort*, serial);
public:
    explicit DLLN3X_Handler(QObject *parent = nullptr);
    Q_INVOKABLE bool send(QString data);

signals:
    void zigbee_recved(QByteArray data, QByteArray sender);

private:
    zigbee_protocol::DLLN3X* _zigbee=nullptr;
    void zigbee_callback(zigbee_protocol::ZigbeeFrame zframe);
    void close();
};

#endif // DLLN3X_HANDLER_H
