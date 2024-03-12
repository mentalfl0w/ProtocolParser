#ifndef PROTOCOL_DATA_H
#define PROTOCOL_DATA_H
#include <QTcpSocket>
#include "typedef.h"

typedef struct SM2_KEY
{
    uint8_t pub_key[129]={0};
    uint8_t pri_key[65]={0};
} SM2_key;

typedef struct SM2_KEY_EXCHANGE
{
    uint8_t session_key[17]={0};
    uint8_t peer_pub_key[65]={0};
} SM2_keyexc;

typedef struct data_frame
{
    u16 head;
    u8 type;
    u8 use_crc;
    u16 data_length;
    u16 crc;
    u8 data[1024];
} data_frame;

#define DATA_FRAME_PREFIX_LEN (sizeof(u16)*3+sizeof(u8)*2)
#define DATA_FRAME_HEAD 0xAAAA
#define new_data_frame(num) \
    struct                  \
{                       \
    u16 head;           \
    u8 type;            \
    u8 use_crc;         \
    u16 data_length;    \
    u16 crc;            \
    u8 data[num];       \
    }

typedef struct digi_env
{
    u16 head;
    u16 length;
    u8 crypted_session_key[112];
    u8 crypted_data[1024];
} digi_env;

#define SM4_PADDING_LEN sizeof(u8)*16
#define DIGI_ENV_PREFIX_LEN sizeof(u16)*2
#define DIGI_ENV_SESSION_KEY_LEN sizeof(u8)*112
#define DIGI_ENV_HEAD 0xAAAB
#define new_digi_env(num)           \
    struct                          \
{                               \
    u16 head;                   \
    u16 length;                 \
    u8 crypted_session_key[112]; \
    u8 crypted_data[num];       \
    }

typedef struct ssl_frame
{
    u16 head;
    u16 length;
    u8 data[1024];
} ssl_frame;

#define SSL_FRAME_PREFIX_LEN sizeof(u16)*2
#define SSL_FRAME_HEAD 0xAAAC
#define new_ssl_frame(num) \
    struct                 \
{                      \
    u16 head;          \
    u16 length;        \
    u8 data[num];      \
    }

typedef struct base_frame
{
    u16 head;
    u16 ori_addr;
    u16 des_addr;
    u16 node_addr;
    u16 id;
    u16 length;
    u8 reset_num;
    u8 rand_num;
    u8 data[2048];
} base_frame;

#define BASE_FRAME_PREFIX_LEN (sizeof(u8)*2+sizeof(u16)*6)
#define BASE_FRAME_HEAD 0xAAAD
#define BASE_FRAME_RESET_NUM 10000
#define new_base_frame(num) \
    struct 							\
{										\
    u16 head;				\
    u16 ori_addr;				\
    u16 des_addr;		\
    u16 node_addr;      \
    u16 id;					\
    u16 length;			\
    u8 reset_num; 		\
    u8 rand_num;				\
    u8 data[num];			\
    }

#define HMAC_FRAME_PREFIX_LEN (sizeof(u16)*2+sizeof(u8))
#define HMAC_FRAME_HEAD 0xAAAE
typedef struct hmac_frame
{
    u16 head;
    u16 length;
    u8 value;
    u8 hmac[33];
} hmac_frame;

#define CRYPTO_ZDATA_FRAME_HEAD 0xAAAF
typedef struct crypto_zdata_frame
{
    u16 head;
    u16 length;
    u16 crc;
    u8 data[100];
}crypto_zdata_frame;

#define CRYPTO_ZDATA_FRAME_PREFIX_LEN sizeof(u16)*2
#define new_crypto_zdata_frame(num) \
    struct 							\
{										\
    u16 head;					\
    u16 length;				\
    u8 data[num];			\
    }

#define RAND_DATA_PACK_PREFIX_LEN sizeof(u16)
typedef struct rand_data_pack
{
    u16 rand_num;
    u8 *data;
}rand_data_pack;

typedef struct device
{
    u16 addr;
    u16 id;
    u8 rand_num;
    u8 verified;
    u8 online;
    u8 logined;
    u8 stage;
    u8 chlg_buf[8];
    u8 ip[30];
    u16 port;
    SM2_keyexc key_pair;
    bool operator == (const device &e){
        return this->addr == e.addr;
    }
} device;

#define MQTT_CLIENT_OFFLINE_DATA_TYPE 0
#define MQTT_CLIENT_OFFLINE_DATA_LENGTH sizeof(u16)
typedef struct mqtt_client_offline_data
{
    u16 addr;
}mqtt_client_offline_data;

#define LOGIN_DATA_TYPE 0
#define LOGIN_DATA_LENGTH (sizeof(int)+sizeof(u8)*32)
typedef struct login_data
{
    int user_id;
    unsigned char passwd_sm3[32];
}login_data;

#define LOGIN_RESP_LENGTH (sizeof(int)*3+sizeof(u8)*20)
typedef struct login_resp
{
    int user_id;
    int gate_addr;
    int identity;
    unsigned char user_name[20];
}login_resp;

#define SENSOR_DATA_TYPE 1
#define SENSOR_DATA_LENGTN sizeof(float)*4
typedef struct sensor_data
{
    float ppm;
    float temp;
    float humi;
    float flare;
}sensor_data;

#define THRESHOLD_DATA_TYPE 2
typedef struct threshold_data
{
    int air[2];
    int temp[2];
    int humi[2];
    int flare[2];
}threshold_data;

typedef struct thumb_member
{
    int id;
    int identity;
    char name[20];
    bool lock;
    time_t time;
}thumb_member;

#define THUMB_MEMBERS_DATA_TYPE 3
#define THUMB_MEMBERS_DATA_PREFIX sizeof(int)*2
typedef struct thumb_members_data
{
    int count;
    thumb_member members[100];
}thumb_members_data;

typedef struct face_member
{
    int id;
    int identity;
    char name[20];
    time_t signin_time;
    time_t signout_time;
}face_member;

#define FACE_MEMBERS_DATA_TYPE 4
#define FACE_MEMBERS_DATA_PREFIX sizeof(int)*2
typedef struct face_members_data
{
    int count;
    int online;
    face_member members[100];
}face_members_data;

#define WARN_IMAGE_FIRE_TYPE 1
#define WARN_IMAGE_PEOPLE_TYPE 2

#define WARN_DATA_TYPE 5
typedef struct warn_data
{
    time_t time;
    int gate;
    int alert_id;
    struct alert
    {
        unsigned char type;
        double threshold_data;
        double now_data;
        bool is_predict;
    }alert;
}warn_data;

#define CLIENT_PHOTO_DATA_TYPE 7
#define CLIENT_PHOTO_DATA_PREFIX (sizeof(int)*3+sizeof(char)*30+sizeof(u8))
typedef struct client_photo_data
{
    int data_length;
    int gate;
    int image_id;
    char name[30];
    u8 type;
    u8 data[51200];
}client_photo_data;

#define new_client_photo_data(num) \
    struct client_photo_data       \
{                           \
    int gate;               \
    int image_id;           \
    int data_length;        \
    char name[30];          \
    u8 type;                \
    u8 data[num];           \
    }

#define ORI_FACE_MEMBER_DATA_TYPE 50
#define ORI_FACE_MEMBER_DATA_PREFIX 2
typedef struct ori_face_members_data
{
    u8 count;
    bool is_offline;
    u8 members[256];
} ori_face_members_data;

#define new_ori_face_members_data(num) \
    struct                             \
{                                  \
    u8 count;                      \
    bool is_offline;               \
    u8 members[num];               \
    }

#define ORI_THUMB_MEMBER_DATA_TYPE 51
typedef struct ori_thumb_member_data
{
    u8 id;
    bool is_open;
}ori_thumb_member_data;

#define PHOTO_DATA_TYPE 52
#define PHOTO_DATA_PREFIX (sizeof(int)+sizeof(char)*30+sizeof(u8))
typedef struct photo_data
{
    int data_length;
    char name[30];
    u8 type;
    u8 data[51200];
}photo_data;

#define new_photo_data(num) \
    struct photo_data       \
{                           \
    int data_length;        \
    char name[30];          \
    u8 type;                \
    u8 data[num];           \
    }

#define FUNC_DATA_TYPE 99
typedef struct func_data
{
    bool sensor;
    bool threshold;
    bool warns;
    bool face_member;
    bool thumb_member;
}func_data;


#endif // PROTOCOL_DATA_H
