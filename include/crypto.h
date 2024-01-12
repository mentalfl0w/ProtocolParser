#ifndef CRYPTO_H
#define CRYPTO_H

#include <QObject>
#include <string.h>
#include <typedef.h>
#include <sm2.h>
#include <sm3.h>
#include <sm4.h>
#include "protocol_data.h"

class Crypto
{
public:
    static Crypto* getInstance();
    static int hex2bytearray(char s[], unsigned char bits[], u32 len);
    static int hex2bytearray(char s[], unsigned char *bits);
    static void bytearray2hex(u8 in_s[], char out_b[], u32 len = 32);
    static void bytearray2hex(ecc_point in_s, char out_b[]);
    static void pubhex2point(char *in_b, ecc_point *out_s);
    static void sm2_key_gen(SM2_key *sm2_keypair,bool buffed=true);
    static void SM2_encrypt(u8 *pub, u8 *M, u32 Mlen, u8 *C, u32 *Clen, s32 mode = 0);
    static void SM2_decrypt(u8 *pri, u8 *C, u32 Clen, u8 *M, u32 *Mlen, s32 mode = 0);
    static void SM2_sign(u8 *pri, u8 *data, u32 datalen, u8 *out);
    static bool SM2_verify(u8 *pub, u8 *data, u32 data_len, u8 *sig);
    static void SM3_HMAC(u8 *key, int keylen,u8 *input, int ilen,u8 output[32] );
    static bool SM4_encrypt(u8 *key_origin, u32 key_len, u8 *in_origin, u32 in_len, u8 *out, u32 *out_len);
    static bool SM4_decrypt(u8 *key_origin, u32 key_len, u8 *in, u32 in_len, u8 *out, u32 *out_len);
    static uint8_t get_rand();
private:
    Crypto();
};

#endif // CRYPTO_H
