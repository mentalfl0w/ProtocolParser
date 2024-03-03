#ifndef ZIGBEEDATARESOLVER_H
#define ZIGBEEDATARESOLVER_H

#include <QObject>
#include <QJsonObject>
#include "zigbeeframe.h"
#include "protocol.h"

class ZigBeeDataResolver : public QObject
{
    Q_OBJECT
public:
    static ZigBeeDataResolver* instance();
public slots:
    void message_parser(QJsonObject message, QString self_addr, QList<uint16_t> allow_list, QList<uint16_t> deny_list, QList<uint16_t> wait_queue);
private:
    explicit ZigBeeDataResolver(QObject *parent = nullptr);
    ~ZigBeeDataResolver();
    Q_DISABLE_COPY_MOVE(ZigBeeDataResolver)
    void data_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo = false);
    void src_port_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo = false);
    void des_port_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo = false);
    void remote_addr_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo = false);
    zigbee_protocol::Protocol* _protocol = nullptr;
    QString _self_addr;
    QMap<uint16_t, QPair<device,device>> nodes;
    QList<uint16_t> _allow_list,_deny_list,_wait_queue;
signals:
    void data_send(QString type, QJsonObject data, QList<uint16_t> allow_list, QList<uint16_t> deny_list, QList<uint16_t> wait_queue);
};

#endif // ZIGBEEDATARESOLVER_H
