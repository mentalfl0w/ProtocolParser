#ifndef ZIGBEEPARSER_H
#define ZIGBEEPARSER_H

#include <QObject>
#include <QQmlEngine>
#include <QMap>
#include <QJsonObject>
#include <QQueue>
#include <zigbeeframe.h>
#include "3rdparty/RibbonUI/lib_source/definitions.h"
#include "protocol.h"
#include "eventsbus.h"
#include "config.h"

class ZigBeeParser : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON
    QML_NAMED_ELEMENT(ZigBeeParser)
    Q_PROPERTY_RW(QList<uint16_t>, allow_list)
    Q_PROPERTY_RW(QList<uint16_t>, deny_list)
    Q_PROPERTY_RW(QList<uint16_t>, wait_queue)
    Q_PROPERTY(QString self_addr READ self_addr FINAL)
    Q_PROPERTY(QString hmac_verify_key READ hmac_verify_key WRITE setHmac_verify_key NOTIFY hmac_verify_keyChanged FINAL)
    Q_PROPERTY(QStringList pre_hmac_verify_key READ pre_hmac_verify_key WRITE setPre_hmac_verify_key NOTIFY pre_hmac_verify_keyChanged FINAL)
public:
    Q_INVOKABLE QString self_addr(){ return _self_addr;};
    QString hmac_verify_key(){ return _protocol->hmac_verify_key;};
    void setHmac_verify_key(QString key){ _config->Set("Protocol","hmac_verify_key",key); _protocol->hmac_verify_key = key;emit hmac_verify_keyChanged();};
    QStringList pre_hmac_verify_key(){ return _protocol->pre_hmac_verify_key;};
    void setPre_hmac_verify_key(QStringList keys){ _config->SetArray("Protocol","pre_hmac_verify_key",keys); _protocol->pre_hmac_verify_key = keys;emit pre_hmac_verify_keyChanged();};
    void data_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo = false);
    void message_parser(QJsonObject message);
    static ZigBeeParser* instance();
    static ZigBeeParser* create(QQmlEngine *qmlEngine, QJSEngine *jsEngine){return instance();}
signals:
    void hmac_verify_keyChanged();
    void pre_hmac_verify_keyChanged();
private:
    explicit ZigBeeParser(QObject *parent = nullptr);
    ~ZigBeeParser();
    void src_port_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo = false);
    void des_port_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo = false);
    void remote_addr_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo = false);
    QMap<uint16_t, QPair<device,device>> nodes;
    Event _event;
    EventsBus *_bus = nullptr;
    zigbee_protocol::Protocol* _protocol = nullptr;
    QString _self_addr = QString::number(SELF_ADDR,16).toUpper();
    Config* _config = nullptr;
};
#endif // ZIGBEEPARSER_H
