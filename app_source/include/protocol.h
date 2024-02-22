#ifndef _M_PROTOCOL_H_
#define _M_PROTOCOL_H_
#include "protocol_data.h"
#include "zigbeeframe.h"
#include "config.h"

#define DEFAULT_SELF_ADDR 0xEEEE //按照实际修改

namespace zigbee_protocol {
class Protocol
{
public:
    static Protocol* getInstance();
    void protocal_wrapper(data_frame *frame, u8 type, u16 length, u8 *data, bool use_crc);
    void base_frame_maker(void *in_frame, base_frame *out_frame, u16 dest_addr,device *dev,u16 node_addr=0);
    bool base_frame_parser(base_frame *in_frame, void **out_frame, device *dev);
    uint16_t crc16_xmodem(const uint8_t *buffer, uint32_t buffer_length);
    bool bytecmp(u8 *a, u8 *b, u16 length);
    bool data_frame_verify(data_frame *frame);
    void load_internal_keypair(int (*hex2bytearray)(char s[], unsigned char bits[], u32 len));
    void HMAC_identify(device *self, device *node, hmac_frame *hframe,
                       void (*sendToserver)(ZigbeeFrame &data),
                       void (*SM3_HMAC)(u8 *key, int keylen,u8 *input, int ilen,u8 output[32] ));
    void HMAC_changeVerifykey(u8 key[16], device* self, device *node, void (*sendTonode)(ZigbeeFrame &data),
                                                   bool (* SM4_encrypt)(u8 *key_origin, u32 key_len, u8 *in_origin, u32 in_len, u8 *out, u32 *out_len));
    void zigbee_data_encrypt(data_frame *data, crypto_zdata_frame *zdata,
                        bool (* SM4_encrypt)(u8 *key_origin, u32 key_len, u8 *in_origin, u32 in_len, u8 *out, u32 *out_len),
                        QString en_key = "");
    bool zigbee_data_dectypt(data_frame *data, crypto_zdata_frame *zdata,
                             bool (* SM4_decrypt)(u8 *key_origin, u32 key_len, u8 *in, u32 in_len, u8 *out, u32 *out_len),
                        QString en_key = "");
    QString hmac_verify_key = "11223344556677888877665544332211";
    QStringList pre_hmac_verify_key;
    u16 self_addr = DEFAULT_SELF_ADDR;
private:
    Protocol();
    ~Protocol();
    Config* _config = nullptr;
};
}
#endif
