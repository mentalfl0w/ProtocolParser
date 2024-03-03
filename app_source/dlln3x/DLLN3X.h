#ifndef _DLLN3X_H_
#define _DLLN3X_H_

#include <QObject>
#include <QSerialPort>
#include "zigbeeframe.h"

namespace zigbee_protocol {
class DLLN3X : public QObject {
    Q_OBJECT

private:
    bool _online = false, _recv_lock = false;
    uint8_t _self_channel = 0;
    uint16_t _self_addr = 0x0000, _self_network_id = 0x0000;
    uint32_t _self_baud_rate = 0;
    const QSerialPort::BaudRate _baud_rate_list[7] = {QSerialPort::Baud2400, QSerialPort::Baud4800, QSerialPort::Baud9600,
                                                       QSerialPort::Baud19200, QSerialPort::Baud38400, QSerialPort::Baud57600, QSerialPort::Baud115200};
    QSerialPort* _DSerial;
    explicit DLLN3X(QObject *parent = nullptr);
    Q_DISABLE_COPY_MOVE(DLLN3X)
    void (*_callback)(ZigbeeFrame& zf) = nullptr;
    int readBytesUntil(uint8_t delimiter, uint8_t* buf, qint64 maxSize);
    void _sleep(int msec);

public:
    enum CONFIG_RW_MASK {READ = 0x00, WRITE = 0x10};
    enum CONFIG { ADDR = 0x01, NETWORKID = 0x02, CHANNEL = 0x03, BAUDRATE = 0x04 , SOFT_REBOOT = 0x10};
    enum CONFIG_RESPONSE {
        DONE = 0x00,
        PORT_REMOTE_ACCESS_DENIED = 0xF0,
        CMD_ERROR = 0xF8,
        PKG_LENGTH_ERROR = 0xF9,
        PKG_DATA_ERROR = 0xFA
    };
    enum PIN_CONTROL{ OUT_HIGH = 0x11, OUT_LOW = 0x10, READ_PIN = 0x12};
    enum PIN{ DLLN3X_PIN4 = 0x44, DLLN3X_PIN5 = 0x45};
    static DLLN3X* instance();
    void init(QSerialPort* DSerial, uint32_t baudrate = 115200);
    ZigbeeFrame recv(bool non_blocked = true);
    bool send(ZigbeeFrame zf);
    bool send_cmd(uint8_t des_port, uint8_t arg, uint8_t* data = nullptr, uint8_t data_length = 0);
    void rled_blink(uint8_t time = 50);
    void loop();
    void setcallback(void (*callback)(ZigbeeFrame& zf));
    uint16_t rw_config(CONFIG arg, uint16_t data = 0, CONFIG_RW_MASK mask = CONFIG_RW_MASK::READ);
    uint8_t read_network_id();
    uint8_t set_network_id(uint16_t network_id);
    uint8_t read_channel();
    uint8_t set_channel(uint8_t channel);
    uint16_t read_addr();
    uint8_t set_addr(uint16_t addr);
    uint32_t read_baudrate();
    uint8_t set_baudrate(uint32_t baud_rate);
    void soft_reboot();
    uint8_t get_link_quality(uint16_t des_addr, uint16_t src_addr = 0x0000);
    enum PIN_CONTROL pin_control(PIN pin, PIN_CONTROL cmd);

signals:
    void recved(ZigbeeFrame zf);
};
}
#endif
