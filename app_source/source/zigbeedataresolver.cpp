#include "zigbeedataresolver.h"
#include "crypto.h"
#include <QRandomGenerator>
#include <QMutex>

ZigBeeDataResolver::ZigBeeDataResolver(QObject *parent)
    : QObject{parent}
{
    _protocol = zigbee_protocol::Protocol::getInstance();
}

ZigBeeDataResolver::~ZigBeeDataResolver()
{

}

ZigBeeDataResolver* ZigBeeDataResolver::instance()
{
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    static ZigBeeDataResolver *singleton = nullptr;
    if (!singleton) {
        singleton = new ZigBeeDataResolver();
    }
    return singleton;
}

void ZigBeeDataResolver::data_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo)
{
    src_port_parser(zframe,is_demo);
    des_port_parser(zframe, is_demo);
    remote_addr_parser(zframe, is_demo);
}

void ZigBeeDataResolver::src_port_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo)
{
    switch (zframe.getSrcPort()) {
    case 0x81:

        break;
    default:
        break;
    }
}

void ZigBeeDataResolver::des_port_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo)
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
    bool addr_check = false, id_check = false, reset_flag_check = false;
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
    addr_check = bframe->des_addr == _protocol->self_addr;
    id_check = node->first.id == 0 || ((bframe->id > node->first.id && bframe->id<=BASE_FRAME_RESET_NUM));
    reset_flag_check = bframe->reset_num==0xDD;
    if (is_demo || addr_check && (id_check||reset_flag_check))
    {
        node->first.addr = bframe->ori_addr;
        node->first.id = bframe->id;
        node->first.rand_num = bframe->rand_num;
        frame = (void*)bframe->data;
    }
    else
    {
        QString error_str;
        QJsonObject object;
        if (!addr_check)
            error_str += "，地址错误，包目的地址为：0x" + QString::number(bframe->des_addr, 16).toUpper();
        if(!(id_check||reset_flag_check))
            error_str += "，包id错误，且重置标志位未标记为生效，当前包ID为：" + QString::number(bframe->id) + "，系统存储ID为" + QString::number(node->first.id);
        object.insert("text",QJsonValue(QString(zdata.toHex(' ').toUpper())));
        object.insert("note_text",QJsonValue("请注意，节点0x"+QString::number(node->first.addr,16).toUpper()+"发送的该数据包损坏"+error_str));
        object.insert("recieved", true);
        object.insert("sender", sender);
        object.insert("type","zigbee_recv_data");
        emit data_send("zigbee_recv_data",object);
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
        emit data_send("zigbee_identify_data_view",object);
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
                emit data_send("zigbee_identify_data_view",object);
                if(!is_demo)
                    emit data_send("serial_port",object);
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
                        new_crypto_zdata_frame(32) zdata;
                        memset(&zdata, 0, sizeof(zdata));
                        _protocol->zigbee_data_encrypt((u8*)latest_key.data(), 16, (crypto_zdata_frame *)&zdata, Crypto::SM4_encrypt, key_str);
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
                        emit data_send("zigbee_identify_data_view",object);
                        if(!is_demo)
                            emit data_send("serial_port",object);
                        return;
                    }
                }
                object.insert("note_text",QJsonValue("节点0x"+QString::number(node->first.addr,16).toUpper()+"验证不通过，请考虑移出认证列表"));
                object.insert("recieved", true);
                object.insert("sender", sender);
                object.insert("type","zigbee_identify_data");
                emit data_send("zigbee_identify_data_view",object);
            }
        }
        else if(_deny_list.contains(node->first.addr))
        {
            object.insert("sender", sender);
            object.insert("text","");
            object.insert("note_text",QJsonValue("节点0x"+QString::number(((base_frame *)zframe.getData().data())->ori_addr,16).toUpper()+"被禁止接入"));
            object.insert("recieved", true);
            object.insert("type","zigbee_identify_data");
            emit data_send("zigbee_identify_data_view",object);
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
                emit data_send("zigbee_identify_data_view",object);
            }
            else
            {
                object.insert("sender", sender);
                object.insert("text","");
                object.insert("note_text",QJsonValue("节点0x"+QString::number(((base_frame *)zframe.getData().data())->ori_addr,16).toUpper()+"已在等待队列"));
                object.insert("recieved", true);
                object.insert("type","zigbee_identify_data");
                emit data_send("zigbee_identify_data_view",object);
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
            QString note_text;
            new_data_frame(72) ndata;
            uint8_t data_len = 0;
            memset(&ndata,0,sizeof(ndata));
            object.insert("sender", sender);
            object.insert("text",QJsonValue(QString(zdata.toHex(' ').toUpper())));
            object.insert("note_text",QJsonValue("收到节点0x"+sender+"发送的数据"));
            object.insert("recieved", true);
            object.insert("type","zigbee_recv_data");
            if (*(u16 *)frame == CRYPTO_ZDATA_FRAME_HEAD)
            {
                note_text = "解密数据为按照未加密传输重新打包的原始数据，因此数据长度会有差异\n";
                new_base_frame(sizeof(ndata)) nbframe;
                memcpy(&nbframe, bframe, BASE_FRAME_PREFIX_LEN);
                zigbee_protocol::ZigbeeFrame nzframe = zframe;
                czdata = (crypto_zdata_frame*)frame;
                _protocol->zigbee_data_dectypt((uint8_t*)&ndata, &data_len, czdata, Crypto::SM4_decrypt);
                memcpy(nbframe.data, &ndata, ndata.data_length + DATA_FRAME_PREFIX_LEN);
                nbframe.length = BASE_FRAME_PREFIX_LEN + ndata.data_length + DATA_FRAME_PREFIX_LEN;
                nzframe.setData((char*)&nbframe,nbframe.length);
                zdata = QByteArray((char *)nzframe.data(), nzframe.size());
                object.insert("decrypted_text", QJsonValue(QString(zdata.toHex(' ').toUpper())));
                ddata = (data_frame*)&ndata;
            }
            else{
                ddata = (data_frame*)frame;
            }
            switch (ddata->type) {
            case SENSOR_DATA_TYPE:
            {
                QStringList name_list, type_list;
                name_list = Config::instance()->getArray("Protocol", "data_frame_name").toStringList();
                type_list = Config::instance()->getArray("Protocol", "data_frame_type").toStringList();
                if (!name_list.length())
                    break;
                note_text += "传感器数据：";
                void* pdata = (void *)ddata->data;
                for (uint8_t i = 0; i < name_list.length(); i++)
                {
                    note_text += name_list[i]+ ":" + sensor_data_reader(&pdata,type_list[i]) + ' ';
                }
                object.insert("note_text",QJsonValue(note_text));
                break;
            }
            default:
                break;
            }
            if (QRandomGenerator::global()->bounded(2)!=0 && is_demo)
                object.insert("decrypted_text", QJsonValue(QString(zdata.toHex(' ').toUpper())));
            emit data_send("zigbee_recv_data_view",object);
        }
        else
        {
            object.insert("sender", sender);
            object.insert("text",QJsonValue(QString(zdata.toHex(' ').toUpper())));
            object.insert("note_text",QJsonValue("收到节点0x"+sender+"发送的数据,但节点并未认证"));
            object.insert("recieved", true);
            object.insert("type","zigbee_recv_data");
            emit data_send("zigbee_recv_data_view",object);
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
            emit data_send("zigbee_recv_data_view",object);
        }
        break;
    }
    default:
        break;
    }
}

void ZigBeeDataResolver::remote_addr_parser(zigbee_protocol::ZigbeeFrame &zframe, bool is_demo)
{
    switch (zframe.getRemoteAddr()) {
    case 0xAAAA:

        break;
    default:
        break;
    }
}

void ZigBeeDataResolver::message_parser(QJsonObject message, QString self_addr)
{
    QMutex mutex;
    mutex.lock();
    _self_addr = self_addr;
    mutex.unlock();
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

QString ZigBeeDataResolver::sensor_data_reader(void **data,QString type)
{
    QString d;
    uint8_t** pdata = (uint8_t**)(data);
    if (type.contains("64"))
    {
        if(type.contains("u"))
            d = QString::number(*(uint64_t*)(*data));
        else
            d = QString::number(*(int64_t*)(*data));
        *pdata += sizeof(uint64_t);
        return d;
    }
    else if (type.contains("32"))
    {
        if(type.contains("u"))
            d = QString::number(*(uint32_t*)(*data));
        else
            d = QString::number(*(int32_t*)(*data));
        *pdata += sizeof(uint32_t);
        return d;
    }
    else if (type.contains("16"))
    {
        if(type.contains("u"))
            d = QString::number(*(uint16_t*)(*data));
        else
            d = QString::number(*(int16_t*)(*data));
        *pdata += sizeof(uint16_t);
        return d;
    }
    else if (type.contains("8"))
    {
        if(type.contains("u"))
            d = QString::number(*(uint8_t*)(*data));
        else
            d = QString::number(*(int8_t*)(*data));
        *pdata += sizeof(uint8_t);
        return d;
    }
    else if (type=="float")
    {
        d = QString::number(*(float*)(*data), 'g', 1);
        *pdata += sizeof(float);
        return d;
    }
    else if (type=="double")
    {
        d = QString::number(*(double*)(*data), 'g', 1);
        *pdata += sizeof(double);
        return d;
    }
    else
        return d;
}
