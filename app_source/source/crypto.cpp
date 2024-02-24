#include "crypto.h"
#include <math.h>

Crypto::Crypto()
{

}

Crypto* Crypto::getInstance()
{
    static Crypto crypto_instance;
    return &crypto_instance;
}

int Crypto::hex2bytearray(char s[], unsigned char bits[], u32 len)
{
    u32 i, n = 0;
    for (i = 0; i < len; i += 2) {
        if (s[i] >= 'a' && s[i] <= 'f') bits[n] = s[i] - 'a' + 10;
        else if (s[i] >= 'A' && s[i] <= 'F') bits[n] = s[i] - 'A' + 10;
        else bits[n] = s[i] - '0';
        if (s[i + 1] >= 'a' && s[i + 1] <= 'f') bits[n] = (bits[n] << 4) | (s[i + 1] - 'a' + 10);
        else if (s[i + 1] >= 'A' && s[i + 1] <= 'F') bits[n] = (bits[n] << 4) | (s[i + 1] - 'A' + 10);
        else bits[n] = (bits[n] << 4) | (s[i + 1] - '0');
        ++n;
    }
    bits[n] = 0;
    return n;
}

int Crypto::hex2bytearray(char s[], unsigned char *bits)
{
    int i=0, n = 0;
    if (s[i] >= 'a' && s[i] <= 'f') *bits = s[i] - 'a' + 10;
    else if (s[i] >= 'A' && s[i] <= 'F') *bits = s[i] - 'A' + 10;
    else *bits = s[i] - '0';
    if (s[i + 1] >= 'a' && s[i + 1] <= 'f') *bits = (*bits << 4) | (s[i + 1] - 'a' + 10);
    else if (s[i + 1] >= 'A' && s[i + 1] <= 'F') *bits = (*bits << 4) | (s[i + 1] - 'A' + 10);
    else *bits = (*bits << 4) | (s[i + 1] - '0');
    ++n;
    return n;
}

void Crypto::bytearray2hex(u8 in_s[], char out_b[], u32 len)
{
    u32 i;
    for (i = 0; i < len; i++) {
        snprintf(out_b + i * 2, 2, "%02X", in_s[i]);
    }
    out_b[len * 2] = 0;
}

void Crypto::bytearray2hex(ecc_point in_s, char out_b[])
{
    int i;
    for (i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            snprintf(out_b + i * 8 * 2 + j * 2, 2, "%02X", (u8)(in_s.x[i] >> (j * 8)));
        }
    }
    for (i = 0; i < 4; i++) {
        for (int j = 0; j < 8; j++) {
            snprintf(out_b + i * 8 * 2 + j * 2 + 64, 2, "%02X", (u8)(in_s.y[i] >> (j * 8)));
        }
    }
    out_b[128] = 0;
}

void Crypto::pubhex2point(char *in_b, ecc_point *out_s)
{
    u64 temp = 0;
    for (int i = 0; i < 4; i++) {
        out_s->x[i] = 0;
        for (int j = 0; j < 8; j++) {
            sscanf(in_b + i * 8 * 2 + j * 2, "%2llX", &temp);
            out_s->x[i] += temp << (j * 8);
        }
    }
    for (int i = 0; i < 4; i++) {
        out_s->y[i] = 0;
        for (int j = 0; j < 8; j++) {
            sscanf(in_b + i * 8 * 2 + j * 2 + 64, "%2llX", &temp);
            out_s->y[i] += temp << (j * 8);
        }
    }
}

void Crypto::sm2_key_gen(SM2_key *sm2_keypair,bool buffed)
{
    SM2_key temp_keypair;
    ecc_point temp_pub_p;
    u8 temp_pub[129]={0},temp_pri[65]={0};
    sm2_make_keypair(temp_pri,&temp_pub_p);
    bytearray2hex(temp_pub_p, (char *)temp_pub);
    hex2bytearray((char *)temp_pub,temp_keypair.pub_key,128);
    strncpy((char *)temp_keypair.pri_key,(char *)temp_pri, 32);
    strncpy((char *)sm2_keypair->pri_key,(char *)temp_keypair.pri_key, 32);
    strncpy((char *)sm2_keypair->pub_key,(char *)temp_keypair.pub_key, 64);
    if (buffed)
    {
        u8 pri_buf[65];
        bytearray2hex(sm2_keypair->pri_key, (char *)pri_buf);
        strncpy((char *)sm2_keypair->pri_key,(char *)pri_buf, 64);
        strncpy((char *)sm2_keypair->pub_key,(char *)temp_pub, 128);
    }
}

void Crypto::SM2_encrypt(u8 *pub, u8 *M, u32 Mlen, u8 *C, u32 *Clen, s32 mode)
{
    ecc_point pub_point;
    memcpy(pub_point.x, pub, 32);
    memcpy(pub_point.y, pub+32, 32);
    sm2_encrypt(&pub_point, (u8 *)M, Mlen, (u8 *)C, Clen);
    if (!mode) {
        u8 *temp_C = (u8 *)malloc(sizeof(u8) * (*Clen));
        memset(temp_C, 0, sizeof(u8) * (*Clen));
        sm2_C1C2C3ConvertToC1C3C2(temp_C, (u8 *)C, *Clen, mode);
        memset(C, 0, sizeof(u8) * (*Clen));
        memcpy(C, temp_C, sizeof(u8) * (*Clen));
        free(temp_C);
    }
}

// Default Input C1|C3|C2
void Crypto::SM2_decrypt(u8 *pri, u8 *C, u32 Clen, u8 *M, u32 *Mlen, s32 mode)
{
    u8 *temp_C = (u8 *)malloc(sizeof(u8) * Clen);
    if (!mode) {
        sm2_C1C2C3ConvertToC1C3C2(temp_C, C, Clen, !mode);
    }
    sm2_decrypt(pri, temp_C, Clen, (u8 *)M, Mlen);
    free(temp_C);
}

// Output r | s
void Crypto::SM2_sign(u8 *pri, u8 *data, u32 datalen, u8 *out)
{
    struct sm3_ctx h;
    u8 hash[33] = { 0 };
    u8 r[33] = { 0 }, s[33] = { 0 };
    sm3_finup(&h, data, datalen, hash);
    sm2_sign(r, s, pri, hash);
    memcpy(out, r, 32);
    memcpy(out+32, s, 32);
}

bool Crypto::SM2_verify(u8 *pub, u8 *data,u32 data_len, u8 *sig)
{
    struct sm3_ctx h;
    u8 hash[33] = { 0 };
    u8 r[33] = { 0 }, s[33] = { 0 };
    ecc_point pub_point;
    memcpy(pub_point.x, pub, 32);
    memcpy(pub_point.y, pub+32, 32);
    sm3_finup(&h, data, data_len, hash);
    memcpy(r, sig, 32);
    memcpy(s, sig+32, 32);
    return (sm2_verify(&pub_point, hash, r, s) == 0 ? true : false);
}

void Crypto::SM3_HMAC(u8 *key, int keylen, u8 *input, int ilen, u8 *output)
{
    sm3_hmac(key, keylen, input, ilen, output);
}

bool Crypto::SM4_encrypt(u8 *key_origin, u32 key_len, u8 *in_origin, u32 in_len, u8 *out, u32 *out_len, bool use_real_cbc)
{
    struct sm4_ctx a;
    u32 max_len;
    u8 key[17] = { 0 }, *in = (u8 *)malloc(sizeof(u8) * (in_len + 16 * 2 + 1)); // 为保证C兼容性，不使用智能指针 两个16分别是为首末尾填充留足留足空间，1是冗余量
    u8 iv[17] = {
                 0x21, 0xBC, 0xC1, 0xEA, 0x0D, 0xB8, 0x54, 0x6D, 0xCE, 0xE4, 0xDB, 0x3C, 0xFA, 0xC1, 0x3C, 0xEF }, fix_len; // 此处为省时才用固定IV，实际上此处IV必须是随机的才可真正保证SM4 CBC模式下加解密的强度
    if (use_real_cbc)
    {
        for (int i = 0; i < 16; i++)
        {
            iv[i] = Crypto::getInstance()->get_rand(); // 使用随机数IV，如果想节省时间可以注释
        }
    }
    if (key_len > 16) {
        free(in);
        return false;
    }
    fix_len = 16 - key_len;
    memcpy(key, key_origin, key_len);
    if (use_real_cbc)
    {
        for (int i = 0; i < 16; i++)
        {
            in[i] = Crypto::getInstance()->get_rand(); // 首填充
        }
    }
    memcpy(in + (use_real_cbc ? 16 : 0), in_origin, in_len);
    if (key_len < 16) {
        for (int i = key_len; i < 16; i++) {
            key[i] = fix_len;
        }
    }
    if (in_len % 16 != 0) {
        max_len = ceil(in_len / 16.0)* 16 + (use_real_cbc ? 16 : 0);
        fix_len = max_len - (use_real_cbc ? 16 : 0) - in_len;
        for (u32 i = in_len + (use_real_cbc ? 16 : 0); i < max_len; i++) {
            in[i] = fix_len;
        }
        in_len = max_len;
    } else {
        for (u32 i = in_len + (use_real_cbc ? 16 : 0); i < in_len + 16 + (use_real_cbc ? 16 : 0); i++) {
            in[i] = 16;
        }
        in_len += 16 + (use_real_cbc ? 16 : 0);
    }
    *out_len = in_len;
    sm4_cbc_encrypt(&a, key, iv, in, in_len, out);
    free(in);
    return true;
}

bool Crypto::SM4_decrypt(u8 *key_origin, u32 key_len, u8 *in, u32 in_len, u8 *out, u32 *out_len, bool use_real_cbc)
{
    struct sm4_ctx b;
    u32 fix_len;
    u8 key[17] = { 0 };
    u8 iv[17] = {
                 0x21, 0xBC, 0xC1, 0xEA, 0x0D, 0xB8, 0x54, 0x6D, 0xCE, 0xE4, 0xDB, 0x3C, 0xFA, 0xC1, 0x3C, 0xEF }; // 此处为省时才用固定IV，实际上此处IV必须是随机的才可真正保证SM4 CBC模式下加解密的强度
    u8* out_buf = (u8* )malloc(in_len * sizeof(u8)); // 为保证C兼容性，不使用智能指针
    if (use_real_cbc)
    {
        for (int i = 0; i < 16; i++)
        {
            iv[i] = Crypto::getInstance()->get_rand(); // 使用随机数IV，如果想节省时间可以注释
        }
    }
    if (key_len > 16) {
        free(out_buf);
        return false;
    }
    fix_len = 16 - key_len;
    memcpy(key, key_origin, key_len);
    if (in_len % 16 != 0) {
        free(out_buf);
        return false;
    }
    if (key_len < 16) {
        for (int i = key_len; i < 16; i++) {
            key[i] = fix_len;
        }
    }
    sm4_cbc_decrypt(&b, key, iv, in, in_len, out_buf);
    fix_len = out_buf[in_len - 1];
    if (fix_len>in_len)
    {
        free(out_buf);
        return false;
    }
    *out_len = in_len - fix_len - (use_real_cbc ? 16 : 0);
    memcpy(out, out_buf + (use_real_cbc ? 16 : 0), *out_len);
    free(out_buf);
    return true;
}

uint8_t Crypto::get_rand()
{
    uint8_t num = 0;
    static time_t seed = 0;
    srand(time(nullptr) + seed);
    seed = rand();
    return rand() % 256;
}
