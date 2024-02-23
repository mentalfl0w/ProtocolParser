#include "zigbeeparser.h"
#include "crypto.h"
#include <QMutex>
#include <QRandomGenerator>

ZigBeeParser::ZigBeeParser(QObject *parent)
    : QObject{parent}
{
    _config = Config::instance();
    _protocol = zigbee_protocol::Protocol::getInstance();
    _bus = EventsBus::instance();
    _event.type("zigbee_parser");
    _bus->reg_event(&_event);
    connect(&_event,&Event::triggered,this,&ZigBeeParser::message_parser);
}

ZigBeeParser::~ZigBeeParser()
{
    _bus->unreg_event(&_event);
    disconnect(&_event,&Event::triggered,this,&ZigBeeParser::message_parser);
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

void ZigBeeParser::data_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo)
{
    src_port_parser(zframe,is_demo);
    des_port_parser(zframe, is_demo);
    remote_addr_parser(zframe, is_demo);
}

void ZigBeeParser::src_port_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo)
{
    switch (zframe.getSrcPort()) {
    case 0x81:

        break;
    default:
        break;
    }
}

void ZigBeeParser::des_port_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo)
{
    if (zframe.size() <= 7)
    {
        qDebug()<<"ZigBee: recved package is corrupted, dropped.";
        return;
    }
    void *frame = nullptr;
    base_frame *bframe=nullptr;
    hmac_frame *hframe=nullptr;
    crypto_zdata_frame *czdata = nullptr;
    data_frame *ddata = nullptr;
    QByteArray zdata((char *)zframe.data(), zframe.size());
    QString sender = QString::number(((base_frame *)zframe.getData().data())->ori_addr,16).toUpper();
    device temp,self;
    QPair<device,device>* node = nullptr;
    memset(&temp,0,sizeof(temp));
    memset(&temp,0,sizeof(self));
    self.addr = _protocol->self_addr;
    if (!nodes.contains(((base_frame *)zframe.getData().data())->ori_addr))
    {
        temp.addr=((base_frame *)zframe.getData().data())->ori_addr;
        temp.id = 0;
        temp.online = 1;
        temp.rand_num = 0;
        temp.verified = 0;
        nodes.insert(temp.addr,qMakePair(temp,self));
    }
    node = &nodes[((base_frame *)zframe.getData().data())->ori_addr];
    bframe = (base_frame*)zframe.getData().data();
    if (is_demo || bframe->des_addr == _protocol->self_addr && (node->first.id == 0||bframe->reset_num==0xDD||((bframe->id > node->first.id && bframe->id<=BASE_FRAME_RESET_NUM))))
    {
        node->first.addr = bframe->ori_addr;
        node->first.id = bframe->id;
        node->first.rand_num = bframe->rand_num;
        frame = (void*)bframe->data;
    }
    else
    {
        QJsonObject object;
        object.insert("text",QJsonValue(QString(zdata.toHex(' ').toUpper())));
        object.insert("note_text",QJsonValue("请注意，节点0x"+QString::number(node->first.addr,16).toUpper()+"发送的该数据包损坏"));
        object.insert("recieved", true);
        object.insert("sender", sender);
        object.insert("type","zigbee_identify_data");
        _bus->push_data("zigbee_identify_data_view",object);
        return;
    }
    switch (zframe.getDesPort()) {
    case 0x81:
    {
        hframe = (hmac_frame*)frame;
        QJsonObject object;
        object.insert("text",QJsonValue(QString(zdata.toHex(' ').toUpper())));
        object.insert("sender", sender);
        object.insert("note_text",QJsonValue("收到节点0x"+ sender +"发送的验证信息"));
        object.insert("recieved", true);
        object.insert("type","zigbee_identify_data");
        _bus->push_data("zigbee_identify_data_view",object);
        if (_allow_list.contains(node->first.addr))
        {
            u8 hmac[33]="";
            QByteArray key = QByteArray::fromHex(_protocol->hmac_verify_key.toLatin1());
            Crypto::getInstance()->SM3_HMAC((u8 *)key.data(),16,&hframe->value,1,hmac);
            if (_protocol->bytecmp(hframe->hmac,hmac,32))
            {
                new_data_frame(15) data;
                memset(&data, 0, sizeof(data));
                _protocol->protocal_wrapper((data_frame *)&data, 0, 10, (u8 *)"Identified", 0);
                node->first.verified = 1;
                new_base_frame(15 + DATA_FRAME_PREFIX_LEN + BASE_FRAME_PREFIX_LEN) bframe;
                memset(&bframe, 0, sizeof(bframe));
                _protocol->base_frame_maker(&data, (base_frame *)&bframe, node->first.addr,&node->second);
                zigbee_protocol::ZigbeeFrame zf(zframe.getSrcPort(),zframe.getDesPort(),zframe.getRemoteAddr(),(uint8_t *)&bframe,bframe.length);
                QByteArray bdata((char *)&bframe, bframe.length);
                QByteArray zdata((char *)zf.data(), zf.size());
                QJsonObject object;
                object.insert("baseframe_data",QJsonValue(QString(bdata.toHex(' ').toUpper())));
                object.insert("text",QJsonValue(QString(zdata.toHex(' ').toUpper())));
                object.insert("note_text",QJsonValue("向0x"+QString::number(node->first.addr,16).toUpper()+"节点发送验证通过回复信息"));
                object.insert("recieved", false);
                object.insert("sender", QString::number(_protocol->self_addr,16).toUpper());
                object.insert("type","zigbee_identify_data");
                _bus->push_data("zigbee_identify_data_view",object);
                if(!is_demo)
                    _bus->push_data("serial_port",object);
            }
            else
            {
                u8 hmac[33]="";
                QJsonObject object;
                QByteArray latest_key = QByteArray::fromHex(_protocol->hmac_verify_key.toLatin1());
                for (auto key_str : _protocol->pre_hmac_verify_key)
                {
                    QByteArray key = QByteArray::fromHex(key_str.toLatin1());
                    Crypto::getInstance()->SM3_HMAC((u8 *)key.data(),16,&hframe->value,1,hmac);
                    if (_protocol->bytecmp(hframe->hmac,hmac,32))
                    {
                        zigbee_protocol::ZigbeeFrame zf(0x82,0x82,node->first.addr);
                        new_data_frame(16) dframe;
                        memset(&dframe, 0, sizeof(dframe));
                        _protocol->protocal_wrapper((data_frame *)&dframe, 0, 16, (u8*)latest_key.data(), 0);
                        new_crypto_zdata_frame(sizeof(dframe)) zdata;
                        memset(&zdata, 0, sizeof(zdata));
                        _protocol->zigbee_data_encrypt((data_frame *)&dframe, (crypto_zdata_frame *)&zdata, Crypto::SM4_encrypt, key_str);
                        new_base_frame(sizeof(zdata) + BASE_FRAME_PREFIX_LEN) bframe;
                        memset(&bframe, 0, sizeof(bframe));
                        _protocol->base_frame_maker(&zdata, (base_frame *)&bframe, node->first.addr, &node->second);
                        zf.setData((u8 *)&bframe, bframe.length);
                        QByteArray bdata((char *)&bframe, bframe.length);
                        QByteArray zfdata((char *)zf.data(), zf.size());
                        object.insert("baseframe_data",QJsonValue(QString(bdata.toHex(' ').toUpper())));
                        object.insert("text",QJsonValue(QString(zfdata.toHex(' ').toUpper())));
                        object.insert("note_text",QJsonValue("节点0x"+QString::number(node->first.addr,16).toUpper()+"正在使用旧密钥，发送密钥更新指令"));
                        object.insert("recieved", false);
                        object.insert("sender", QString::number(_protocol->self_addr,16).toUpper());
                        object.insert("type","zigbee_identify_data");
                        _bus->push_data("zigbee_identify_data_view",object);
                        if(!is_demo)
                            _bus->push_data("serial_port",object);
                        return;
                    }
                }
                object.insert("note_text",QJsonValue("节点0x"+QString::number(node->first.addr,16).toUpper()+"验证不通过，请考虑移出认证列表"));
                object.insert("recieved", true);
                object.insert("sender", sender);
                object.insert("type","zigbee_identify_data");
                _bus->push_data("zigbee_identify_data_view",object);
            }
        }
        else if(_deny_list.contains(node->first.addr))
        {
            object.insert("sender", sender);
            object.insert("text","");
            object.insert("note_text",QJsonValue("节点0x"+QString::number(((base_frame *)zframe.getData().data())->ori_addr,16).toUpper()+"被禁止接入"));
            object.insert("recieved", true);
            object.insert("type","zigbee_identify_data");
            _bus->push_data("zigbee_identify_data_view",object);
        }
        else {
            if (!_wait_queue.contains(node->first.addr))
            {
                _wait_queue.append(node->first.addr);
                object.insert("sender", sender);
                object.insert("text","");
                object.insert("note_text",QJsonValue("节点0x"+QString::number(((base_frame *)zframe.getData().data())->ori_addr,16).toUpper()+"进入等待队列"));
                object.insert("recieved", true);
                object.insert("type","zigbee_identify_data");
                _bus->push_data("zigbee_identify_data_view",object);
            }
            else
            {
                object.insert("sender", sender);
                object.insert("text","");
                object.insert("note_text",QJsonValue("节点0x"+QString::number(((base_frame *)zframe.getData().data())->ori_addr,16).toUpper()+"已在等待队列"));
                object.insert("recieved", true);
                object.insert("type","zigbee_identify_data");
                _bus->push_data("zigbee_identify_data_view",object);
            }
        }
        break;
    }
    case 0x82:
    {

    }
    case 0x83:
    {
        QJsonObject object;
        if (node->first.verified)
        {
            zigbee_protocol::ZigbeeFrame dzf = zframe;
            new_data_frame(72) ndata;
            memset(&ndata,0,sizeof(ndata));
            if (*(u16 *)frame == CRYPTO_ZDATA_FRAME_HEAD)
            {
                czdata = (crypto_zdata_frame*)frame;
                _protocol->zigbee_data_dectypt((data_frame*)&ndata, czdata, Crypto::SM4_decrypt);
                dzf.setData((char*)&ndata,ndata.data_length + DATA_FRAME_PREFIX_LEN);
                zdata = QByteArray((char *)dzf.data(), dzf.size());
                object.insert("decrypted_text", QJsonValue(QString(zdata.toHex(' ').toUpper())));
            }
            object.insert("sender", sender);
            object.insert("text",QJsonValue(QString(zdata.toHex(' ').toUpper())));
            object.insert("note_text",QJsonValue("收到节点0x"+sender+"发送的数据"));
            object.insert("recieved", true);
            object.insert("type","zigbee_recv_data");
            if (QRandomGenerator::global()->bounded(2) && is_demo)
                object.insert("decrypted_text", QJsonValue(QString(zdata.toHex(' ').toUpper())));
            _bus->push_data("zigbee_recv_data_view",object);
        }
        else
        {
            object.insert("sender", sender);
            object.insert("text",QJsonValue(QString(zdata.toHex(' ').toUpper())));
            object.insert("note_text",QJsonValue("收到节点0x"+sender+"发送的数据,但节点并未认证"));
            object.insert("recieved", true);
            object.insert("type","zigbee_recv_data");
            _bus->push_data("zigbee_recv_data_view",object);
            node->second.id=0;
            new_data_frame(5) dframe;
            memset(&dframe,0,sizeof (dframe));
            _protocol->protocal_wrapper((data_frame *)&dframe, 0, 5, (u8 *)"RESET", false);
            new_base_frame(5 + DATA_FRAME_PREFIX_LEN + BASE_FRAME_PREFIX_LEN) bframe;
            memset(&bframe, 0, sizeof(bframe));
            _protocol->base_frame_maker(&dframe, (base_frame *)&bframe, node->first.addr,&node->second);
            zigbee_protocol::ZigbeeFrame zf(0x83,0x83,node->first.addr,(char *)&bframe,bframe.length);
            QByteArray zfdata((char *)zf.data(),zf.size());
            QJsonObject object;
            object.insert("sender", QString::number(_protocol->self_addr,16).toUpper());
            object.insert("text",QJsonValue(QString(zfdata.toHex(' ').toUpper())));
            object.insert("note_text",QJsonValue("向节点0x"+sender+"发送重置命令"));
            object.insert("recieved", false);
            object.insert("type","zigbee_recv_data");
            _bus->push_data("zigbee_recv_data_view",object);
        }
    }
    default:
        break;
    }
}

void ZigBeeParser::remote_addr_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo)
{
    switch (zframe.getRemoteAddr()) {
    case 0xAAAA:

        break;
    default:
        break;
    }
}

void ZigBeeParser::message_parser(QJsonObject message)
{
    if (message["type"] == "demo_verify_request")
    {
        hmac_frame frame;
        frame.value = QRandomGenerator::global()->bounded(256);
        frame.head = HMAC_FRAME_HEAD;
        frame.length = 32 + HMAC_FRAME_PREFIX_LEN;
        QByteArray key = QByteArray::fromHex(_protocol->hmac_verify_key.toLatin1());
        Crypto::getInstance()->SM3_HMAC((u8 *)key.data(), 16, &frame.value, 1, frame.hmac);
        new_base_frame(50 + BASE_FRAME_PREFIX_LEN) bframe;
        device test;
        test.addr = 0x5656;
        _protocol->base_frame_maker(&frame, (base_frame *)&bframe, _protocol->self_addr, &test);
        zigbee_protocol::ZigbeeFrame zf(0x81,0x81,0x5656,(char*)&bframe,bframe.length);
        data_parser(zf, true);
    }
    if (message["type"] == "demo_verify_key_update")
    {
        hmac_frame frame;
        frame.value = QRandomGenerator::global()->bounded(256);
        frame.head = HMAC_FRAME_HEAD;
        frame.length = 32 + HMAC_FRAME_PREFIX_LEN;
        QByteArray key = QByteArray::fromHex(_protocol->pre_hmac_verify_key[QRandomGenerator::global()->bounded(_protocol->pre_hmac_verify_key.length())].toLatin1());
        Crypto::getInstance()->SM3_HMAC((u8 *)key.data(), 16, &frame.value, 1, frame.hmac);
        new_base_frame(50 + BASE_FRAME_PREFIX_LEN) bframe;
        device test;
        test.addr = 0x5656;
        _protocol->base_frame_maker(&frame, (base_frame *)&bframe, _protocol->self_addr, &test);
        zigbee_protocol::ZigbeeFrame zf(0x81,0x81,0x5656,(char*)&bframe,bframe.length);
        data_parser(zf, true);
    }
    if (message["type"] == "demo_recv_data" || message["type"] == "zigbee_raw_data")
    {
        QString data = message["data_hex"].toString();
        QByteArray bdata;
        QStringList td = data.split(' ');
        for (auto item : td)
            bdata += QByteArray::fromHex(item.toLatin1());
        zigbee_protocol::ZigbeeFrame zf;
        zf.load_package((uint8_t*)bdata.data(),bdata.length());
        data_parser(zf, message["type"] == "demo_recv_data");
    }
}
