#include "protocol.h"


using namespace zigbee_protocol;

Protocol::Protocol(){
    _config = Config::instance();
    if (_config->get("Protocol","hmac_verify_key").toString().isEmpty())
        _config->set("Protocol","hmac_verify_key",hmac_verify_key);
    else
        hmac_verify_key = _config->get("Protocol","hmac_verify_key").toString();
    if (_config->getArray("Protocol","pre_hmac_verify_key").toStringList().isEmpty())
        _config->setArray("Protocol","pre_hmac_verify_key",pre_hmac_verify_key);
    else
        pre_hmac_verify_key = _config->getArray("Protocol","pre_hmac_verify_key").toStringList();
}

Protocol::~Protocol()
{
}

Protocol* Protocol::getInstance()
{
    static Protocol protocol_instance;
    return &protocol_instance;
}

void Protocol::protocal_wrapper(data_frame *frame, u8 type, u16 length, u8 *data, bool use_crc)
{
    frame->head=DATA_FRAME_HEAD;
    frame->type=type;
    frame->data_length = length;
    if (use_crc)
    {
        frame->use_crc=0xDD;
        u16 crc = crc16_xmodem(data, length);
        frame->crc = crc;
    }
    else
    {
        frame->use_crc=0xFF;
        frame->crc = 0xFFFF;
    }
    memcpy(frame->data,data,length);
}

void Protocol::base_frame_maker(void *in_frame, base_frame *out_frame, u16 dest_addr,device *dev,u16 node_addr)
{
    out_frame->head = BASE_FRAME_HEAD;
    out_frame->ori_addr= dev->addr;
    out_frame->des_addr = dest_addr;
    out_frame->node_addr = node_addr;
    if (dev->id < BASE_FRAME_RESET_NUM)
    {
        if (dev->id == 0)
        {
            out_frame->reset_num = 0xFF;
            do{
                srand(static_cast<u16>(time(nullptr)));
                out_frame->rand_num = rand() & 0xFF;
            }while(out_frame->rand_num==0);
            dev->rand_num = out_frame->rand_num;
            out_frame->reset_num = 0xDD;
        }
        else
        {
            out_frame->rand_num = dev->rand_num;
            out_frame->reset_num = 0xFF;
        }
        out_frame->id = ++(dev->id);
    }
    else
    {
        out_frame->id = dev->id = 0;
        out_frame->reset_num = 0xDD;
        do{
            srand(static_cast<u16>(time(nullptr)));
            out_frame->rand_num = rand() & 0xFF;
        }while(out_frame->rand_num==0);
    }
    switch (*(u16 *)in_frame)
    {
    case 0xAAAB:
    {
        out_frame->length = ((digi_env *)in_frame)->length + BASE_FRAME_PREFIX_LEN;
        break;
    }
    case 0xAAAC:
    {
        out_frame->length = ((ssl_frame *)in_frame)->length + BASE_FRAME_PREFIX_LEN;
        break;
    }
    case 0xAAAA:
    {
        out_frame->length = ((data_frame *)in_frame)->data_length + DATA_FRAME_PREFIX_LEN + BASE_FRAME_PREFIX_LEN;
        break;
    }
    case 0xAAAE:
    {
        out_frame->length = ((hmac_frame *)in_frame)->length + BASE_FRAME_PREFIX_LEN;
        break;
    }
    case 0xAAAF:
    {
        out_frame->length = ((crypto_zdata_frame *)in_frame)->length + BASE_FRAME_PREFIX_LEN;
        break;
    }
    }
    memcpy(out_frame->data, in_frame ,out_frame->length - BASE_FRAME_PREFIX_LEN);
}

bool Protocol::base_frame_parser(base_frame *in_frame, void **out_frame , device *dev)
{
    if (in_frame->des_addr == self_addr && (dev->id == 0||in_frame->reset_num==0xDD||((in_frame->id > dev->id && in_frame->id<=BASE_FRAME_RESET_NUM))))
    {
        dev->addr = in_frame->ori_addr;
        dev->id = in_frame->id;
        dev->rand_num = in_frame->rand_num;
        *out_frame = (void *)in_frame->data;
        return true;
    }
    else
        return false;
}

uint16_t Protocol::crc16_xmodem(const uint8_t *buffer, uint32_t buffer_length)
{
    uint8_t c, treat, bcrc;
    uint16_t wcrc = 0;

    for(uint32_t i = 0; i < buffer_length; i++)
    {
        c = buffer[i];
        for(uint8_t j = 0; j < 8; j++)
        {
            treat = c & 0x80;
            c <<= 1;
            bcrc = (wcrc >> 8) & 0x80;
            wcrc <<= 1;
            if(treat != bcrc)
                wcrc ^= 0x1021;
        }
    }
    return wcrc;
}

bool Protocol::bytecmp(u8 *a, u8 *b, u16 length)
{
    return memcmp(a,b,length)==0;
}

bool Protocol::data_frame_verify(data_frame *frame)
{
    if(frame->use_crc!=0xFF)
        return !(frame->crc-crc16_xmodem(frame->data,frame->data_length));
    else
        return true;
}

void Protocol::zigbee_data_encrypt(uint8_t *data, uint8_t data_len, crypto_zdata_frame *zdata,
                                     bool (* SM4_encrypt)(u8 *key_origin, u32 key_len, u8 *in_origin, u32 in_len, u8 *out, u32 *out_len, bool use_real_cbc),
                                   QString en_key)
{
    u32 len;
    zdata->head=CRYPTO_ZDATA_FRAME_HEAD;
    QByteArray key = QByteArray::fromHex(en_key == "" ? hmac_verify_key.toLatin1() : en_key.toLatin1());
    SM4_encrypt((u8 *)key.data(), 16, data, data_len, zdata->data,&len,false);
    zdata->length = len + CRYPTO_ZDATA_FRAME_PREFIX_LEN;
    zdata->crc = crc16_xmodem(data, data_len);
}

bool Protocol::zigbee_data_dectypt(uint8_t *data, uint8_t *data_len, crypto_zdata_frame *zdata,
                                       bool (* SM4_decrypt)(u8 *key_origin, u32 key_len, u8 *in, u32 in_len, u8 *out, u32 *out_len, bool use_real_cbc),
                                   QString en_key)
{
    int total_len = zdata->length;
    if (total_len - CRYPTO_ZDATA_FRAME_PREFIX_LEN<=0)
        return false;
    u32 len, msglen = total_len - CRYPTO_ZDATA_FRAME_PREFIX_LEN;
    QByteArray key = QByteArray::fromHex(en_key == "" ? hmac_verify_key.toLatin1() : en_key.toLatin1());
    SM4_decrypt((u8 *)key.data(), 16, zdata->data, msglen, data, &len,false);
    *data_len = len;
    u16 crc = crc16_xmodem(data, *data_len);
    if (crc != zdata->crc)
        return false;
    return true;
}

void Protocol::HMAC_identify(device *self, device *node, hmac_frame *hframe,
                             void (*sendTonode)(ZigbeeFrame &data),
                             void (*SM3_HMAC)(u8 *key, int keylen,u8 *input, int ilen,u8 output[32] ))
{
    if (hframe==NULL)
        return;
    u8 hmac[33]="";
    ZigbeeFrame zf(0x81,0x81,node->addr);
    QByteArray key = QByteArray::fromHex(hmac_verify_key.toLatin1());
    SM3_HMAC((u8 *)key.data(),16,&hframe->value,1,hmac);
    if (bytecmp(hframe->hmac,hmac,32))
    {
        new_data_frame(15) data;
        memset(&data, 0, sizeof(data));
        protocal_wrapper((data_frame *)&data, 0, 10, (u8 *)"Identified", 0);
        node->verified = 1;
        new_base_frame(15 + BASE_FRAME_PREFIX_LEN) bframe;
        memset(&bframe, 0, sizeof(bframe));
        base_frame_maker(&data, (base_frame *)&bframe, node->addr, self);
        zf.setData((u8 *)&bframe, bframe.length);
        sendTonode(zf);
        printf("Node %X had passed HMAC identification.", node->addr);
    }
}

void Protocol::HMAC_changeVerifykey(u8 key[16], device* self, device *node, void (*sendTonode)(ZigbeeFrame &data),
                                    bool (* SM4_encrypt)(u8 *key_origin, u32 key_len, u8 *in_origin, u32 in_len, u8 *out, u32 *out_len, bool use_real_cbc))
{
    ZigbeeFrame zf(0x82,0x82,node->addr);
    new_crypto_zdata_frame(48) zdata;
    memset(&zdata, 0, sizeof(zdata));
    zigbee_data_encrypt(key, 16, (crypto_zdata_frame *)&zdata, SM4_encrypt);
    new_base_frame(48 + BASE_FRAME_PREFIX_LEN) bframe;
    memset(&bframe, 0, sizeof(bframe));
    base_frame_maker(&zdata, (base_frame *)&bframe, node->addr, self);
    zf.setData((u8 *)&bframe, bframe.length);
    sendTonode(zf);
    printf("Node %X's verify key has changed.", node->addr);
}
