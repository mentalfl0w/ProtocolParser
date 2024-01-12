#include "DLLN3X.h"
#include <QTime>
#include <QCoreApplication>

using namespace zigbee_protocol;

DLLN3X::DLLN3X(QObject *parent)
    : QObject{parent}
{

}

DLLN3X* DLLN3X::instance(){
    static DLLN3X instance;
    return &instance;
}

void DLLN3X::init(QSerialPort* DSerial, uint32_t baudrate)
{
    _DSerial = DSerial;
    uint8_t i = 0;
    for (; i < 7; i++)
        if (baudrate == _baud_rate_list[i])
        {
            i = 20;
            break;
        }
    if (i != 20)
        return;
    _DSerial->setBaudRate(baudrate);
    _DSerial->setDataBits(QSerialPort::Data8);
    _DSerial->setParity(QSerialPort::NoParity);
    _DSerial->setStopBits(QSerialPort::OneStop);
    if (!_DSerial->isOpen())
    {
        if (!_DSerial->open(QIODevice::ReadWrite))
            return;
    }
    rled_blink();
    read_addr();
    read_baudrate();
    read_channel();
    read_network_id();
}

ZigbeeFrame DLLN3X::recv(bool non_blocked)
{
    uint8_t buf[66] = "", length = 0;
    ZigbeeFrame zf;
    if (non_blocked) {
        if (_DSerial->bytesAvailable() < 6)
            return zf;
    }
    _recv_lock = true;
    length = readBytesUntil(0xFF, buf, 65);
    _recv_lock = false;
    zf.load_package(buf, length);
    return zf;
}

bool DLLN3X::send(ZigbeeFrame zf)
{
    bool status = false;
    if (zf.getSrcPort() < 0x80)
        return false;
    status = _DSerial->write((char *)zf.data(),zf.size());
    return status;
}

bool DLLN3X::send_cmd(uint8_t des_port, uint8_t arg, uint8_t* data, uint8_t data_length)
{
    ZigbeeFrame zf(0x80, des_port, 0x0000);
    if (data_length - 4 > 63 - 1)
        return false;
    zf.append(arg);
    zf.addData(data, data_length);
    return send(zf);
}

void DLLN3X::rled_blink(uint8_t time)
{
    uint8_t gap = 5;
    // for (int i = 0; i < time; i += gap) {
    //     if (i % 2 != 0)
    //         send_cmd(0x20, gap);
    //     else
    //         _sleep(gap * 250);
    // }
    send_cmd(0x20, gap);
}

uint16_t DLLN3X::read_addr()
{
    if (_self_addr != 0)
        return _self_addr;
    _self_addr = rw_config(CONFIG::ADDR);
    return _self_addr;
}

uint8_t DLLN3X::set_addr(uint16_t addr)
{
    uint8_t resp = rw_config(CONFIG::ADDR, addr, CONFIG_RW_MASK::WRITE);
    if (resp == CONFIG_RESPONSE::DONE)
    {
        _self_addr = addr;
        soft_reboot();
    }
    return resp;
}

uint32_t DLLN3X::read_baudrate()
{
    if (_self_baud_rate != 0)
        return _self_baud_rate;
    uint8_t arg = rw_config(CONFIG::BAUDRATE);
    _self_baud_rate = _baud_rate_list[arg];
    return _self_baud_rate;
}

uint8_t DLLN3X::set_baudrate(uint32_t baud_rate)
{
    uint8_t arg = 0;
    uint8_t i = 0;
    for (; i < 13; i++)
    {
        if (baud_rate == _baud_rate_list[i])
        {
            arg = i;
            break;
        }
    }
    if (arg != i)
        return CONFIG_RESPONSE::CMD_ERROR;
    uint8_t resp = rw_config(CONFIG::BAUDRATE, arg, CONFIG_RW_MASK::WRITE);
    if (resp == CONFIG_RESPONSE::DONE)
    {
        _self_baud_rate = baud_rate;
        soft_reboot();
    }
    return resp;
}

uint8_t DLLN3X::read_network_id()
{
    if (_self_network_id != 0)
        return _self_network_id;
    _self_network_id = rw_config(CONFIG::NETWORKID);
    return _self_network_id;
}

uint8_t DLLN3X::set_network_id(uint16_t network_id)
{
    uint8_t resp = rw_config(CONFIG::NETWORKID, network_id, CONFIG_RW_MASK::WRITE);
    if (resp == CONFIG_RESPONSE::DONE)
    {
        _self_network_id = network_id;
        soft_reboot();
    }
    return resp;
}

uint8_t DLLN3X::read_channel()
{
    if (_self_channel != 0)
        return _self_channel;
    _self_channel = rw_config(CONFIG::CHANNEL);
    return _self_channel;
}

uint8_t DLLN3X::set_channel(uint8_t channel)
{
    uint8_t resp = rw_config(CONFIG::CHANNEL, channel, CONFIG_RW_MASK::WRITE);
    if (resp == CONFIG_RESPONSE::DONE)
    {
        _self_channel = channel;
        soft_reboot();
    }
    return resp;
}

uint16_t DLLN3X::rw_config(CONFIG arg, uint16_t data, CONFIG_RW_MASK mask)
{
    uint8_t cmd_arg = arg|mask;
    if (mask == CONFIG_RW_MASK::WRITE)
    {
        if (arg == CONFIG::ADDR || arg == CONFIG::NETWORKID)
        {
            send_cmd(0x21, cmd_arg, (uint8_t *)&data, 2);
        }
        else
            send_cmd(0x21, cmd_arg, (uint8_t *)&data, 1);
    }
    else
        send_cmd(0x21, cmd_arg);
    ZigbeeFrame zf = recv(false);
    if (zf.getSrcPort() != 0x21 || zf.getDesPort() != 0x80 || zf.getRemoteAddr() != 0x0000)
        return 0;
    switch (arg) {
    case CONFIG::ADDR: {
        if (zf.getData()[0] != 0x21) {
            if (zf.getData()[0] != CONFIG_RESPONSE::DONE)
            {
                printf("DLLN3X write config error: 0x");
                printf("%X\n", zf.getData()[0]);
            }
            return zf.getData()[0];
        } else
            return (zf.getData()[2] << 8) | zf.getData()[1];
    }
    case CONFIG::NETWORKID: {
        if (zf.getData()[0] != 0x22) {
            if (zf.getData()[0] != CONFIG_RESPONSE::DONE)
            {
                printf("DLLN3X write config error: 0x");
                printf("%X\n", zf.getData()[0]);
            }
            return zf.getData()[0];
        } else
            return (zf.getData()[2] << 8) | zf.getData()[1];
    }
    case CONFIG::CHANNEL: {
        if (zf.getData()[0] != 0x23) {
            if (zf.getData()[0] != CONFIG_RESPONSE::DONE)
            {
                printf("DLLN3X write config error: 0x");
                printf("%X\n", zf.getData()[0]);
            }
            return zf.getData()[0];
        } else
            return zf.getData()[1];
    }
    case CONFIG::BAUDRATE: {
        if (zf.getData()[0] != 0x24) {
           if (zf.getData()[0] != CONFIG_RESPONSE::DONE)
            {
                printf("DLLN3X write config error: 0x");
                printf("%X\n", zf.getData()[0]);
            }
            return zf.getData()[0];
        } else
            return zf.getData()[1];
    }
    default:
        return CONFIG_RESPONSE::CMD_ERROR;
    }
}

void DLLN3X::soft_reboot()
{
    send_cmd(0x21, CONFIG::SOFT_REBOOT);
    _self_addr = 0;
    _self_baud_rate = 0;
    _self_channel = 0;
    _self_network_id = 0;
}

uint8_t DLLN3X::get_link_quality(uint16_t des_addr, uint16_t src_addr)
{
    if (des_addr == 0x0000 || des_addr == 0xFFFF)
        return CONFIG_RESPONSE::CMD_ERROR;
    ZigbeeFrame zf(0x80, 0x23, src_addr), r_zf;
    zf.addData((uint8_t *)&des_addr, 2);
    r_zf = recv(false);
    uint16_t pkg_des_addr = (r_zf.getData()[1] << 8) | r_zf.getData()[0];
    if (pkg_des_addr != des_addr)
        return CONFIG_RESPONSE::PKG_DATA_ERROR;
    else
        return r_zf.getData()[2];
}

enum DLLN3X::PIN_CONTROL DLLN3X::pin_control(PIN pin, PIN_CONTROL cmd)
{
    send_cmd(pin, cmd);
    if (cmd == PIN_CONTROL::READ_PIN)
    {
        ZigbeeFrame zf = recv(false);
        return PIN_CONTROL(zf.getData()[0] ^ 0x10);
    }
    return cmd;
}

void DLLN3X::loop()
{
    if (_DSerial->bytesAvailable() > 7 && !_recv_lock) {
        ZigbeeFrame zf = recv();
        printf("Message: ");
        for (int i = 0; i < zf.size(); i++) {
            printf("%02X ", zf[i]);
        }
        printf("at port %02X from %04X:%02X.\n", zf.getDesPort(), zf.getRemoteAddr(), zf.getSrcPort());
        if (zf.getSrcPort() == 0x22)
        {
            printf("DLLN3X send msg error: 0x");
            printf("%X",zf.getData()[0]);
            printf(", Send data to incorrect port: 0x");
            printf("%X\n",zf.getData()[1]);
        }
        emit recved(zf);
    }
}

int DLLN3X::readBytesUntil(uint8_t delimiter, uint8_t* buffer, qint64 maxSize)
{
    QByteArray data;
    qint64 bytesRead = 0;

    while (bytesRead < maxSize)
    {
        if (_DSerial->bytesAvailable())
        {
            qint64 bytesReadNow = _DSerial->read((char *)(buffer) + bytesRead, 1);
            bytesRead += bytesReadNow;

            data.append((char *)(buffer) + bytesRead - bytesReadNow, bytesReadNow);

            if (data.contains(delimiter))
            {
                break;
            }
        }
        else
        {
            break;
        }
    }
    return bytesRead;
}

void DLLN3X::_sleep(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < dieTime )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void DLLN3X::setcallback(void (*callback)(ZigbeeFrame& zf)) { _callback = callback; }
