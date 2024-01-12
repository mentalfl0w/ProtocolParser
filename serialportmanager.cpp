#include "serialportmanager.h"
#include <QSerialPortInfo>
#include <QMutex>

SerialPortManager::SerialPortManager(QObject *parent)
    : QObject{parent}
{
    _zigbee = zigbee_protocol::DLLN3X::instance();
    _bus = EventsBus::instance();
    ZigBeeParser::instance(); // 让ZigBeeParser在QML引擎中实例化
    _event.type("serial_port");
    _bus->reg_event(&_event);
    closed(true);
    opened(false);
    handledBymanager(true);
    recv_hex(false);
    send_hex(false);
    send_type(SendType::Blank);

    _serial_port_scanner.setInterval(1000);
    _serial_port_scanner.start();
    connect(&_serial_port_scanner, &QTimer::timeout, this, [=](){
        available_ports();
    });
    connect(this, &SerialPortManager::handledBymanagerChanged, this, [=](){
        if (!_handledBymanager && _serial_port.isOpen())
        {
            _zigbee->init(&_serial_port, _serial_port.baudRate());
            connect(_zigbee, &zigbee_protocol::DLLN3X::recved, this, &SerialPortManager::zigbee_callback);
        }
        else
        {
            disconnect(_zigbee, &zigbee_protocol::DLLN3X::recved, this, &SerialPortManager::zigbee_callback);
        }
    });
    connect(&_event, &Event::triggered, this, [=](QJsonObject data){
        write(data["text"].toString());
    });
}

SerialPortManager::~SerialPortManager()
{
    close();
    _bus->unreg_event(&_event);
}

SerialPortManager* SerialPortManager::instance(){
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    static SerialPortManager *singleton = nullptr;
    if (!singleton) {
        singleton = new SerialPortManager();
    }
    return singleton;
}

QList<QString> SerialPortManager::available_ports(){
    QList<QString> newPortStringList;
    foreach(const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        newPortStringList += info.portName();
    if (_available_ports_count != newPortStringList.count())
    {
        available_ports_count(newPortStringList.count());
        emit available_portsChanged(newPortStringList);
    }
    return newPortStringList;
}

bool SerialPortManager::open(QString port_name, QString baudrate, QString databits,
                             QString parity, QString stopbits)
{
    _serial_port.setPortName(port_name);
    if (_serial_port.isOpen())
    {
        close();
        return false;
    }
    else
    {
        if (_serial_port.open(QIODevice::ReadWrite))
        {
            _serial_port.setBaudRate(baudrate.toInt());
            switch (databits.toInt())
            {
            case 5:
                _serial_port.setDataBits( QSerialPort::Data5 ); // 设置数据位(数据位为5位)
                break;
            case 6:
                _serial_port.setDataBits( QSerialPort::Data6 ); // 设置数据位(数据位为6位)
                break;
            case 7:
                _serial_port.setDataBits( QSerialPort::Data7 ); // 设置数据位(数据位为7位)
                break;
            case 8:
                _serial_port.setDataBits( QSerialPort::Data8 ); // 设置数据位(数据位为8位)
                break;
            default:
                return false;
            }
            if (parity=="NONE")
            {
                _serial_port.setParity( QSerialPort::NoParity ); // 设置校验位(无校验位)
            }
            else if (parity=="EVEN")
            {
                _serial_port.setParity( QSerialPort::EvenParity ); // 设置校验位(奇校验位)
            }
            else
            {
                _serial_port.setParity( QSerialPort::OddParity ); // 设置校验位(偶校验位)
            }
            if (stopbits=="1")
            {
                _serial_port.setStopBits( QSerialPort::OneStop ); // 设置停止位(停止位为1)
            }
            else if (stopbits=="1.5")
            {
                _serial_port.setStopBits( QSerialPort::OneAndHalfStop ); // 设置停止位(停止位为1.5)
            }
            else
            {
                _serial_port.setStopBits( QSerialPort::TwoStop ); // 设置停止位(停止位为2)
            }
            _serial_port.setFlowControl( QSerialPort::HardwareControl ); // 设置流控制(硬件数据流控制)
            // 当下位机中有数据发送过来时就会响应这个槽函数
            connect(&_serial_port,&QSerialPort::readyRead,this,&SerialPortManager::_ready_for_read);
            connect(&_serial_port,&QSerialPort::aboutToClose,this,&SerialPortManager::close);
            opened(true);
            closed(false);
            return true;
        }
        else
            return false;
    }
}

void SerialPortManager::close()
{
    disconnect(&_serial_port,&QSerialPort::aboutToClose,this,&SerialPortManager::close);
    disconnect(&_serial_port,&QSerialPort::readyRead,this,&SerialPortManager::_ready_for_read);
    disconnect(_zigbee, &zigbee_protocol::DLLN3X::recved, this, &SerialPortManager::zigbee_callback);
    if(_opened)
    {
        _serial_port.clear();
        _serial_port.close();
    }
    readyRead(false);
    closed(true);
    opened(false);
}

QByteArray SerialPortManager::read()
{
    QByteArray bdata;
    if (_opened && _handledBymanager)
    {
        readyRead(false);
        bdata = _serial_port.readAll();
        if (_recv_hex)
        {
            return bdata.toHex(' ').toUpper();
        }
        else
            return bdata;
    }
    else
        return bdata;
}

void SerialPortManager::_ready_for_read()
{
    if (_handledBymanager)
        readyRead(true);
    if (!_handledBymanager)
        _zigbee->loop();
}

bool SerialPortManager::write(QString data)
{
    if (_closed)
        return false;

    if ( !_handledBymanager)
    {
        QByteArray bdata;
        QStringList td = data.split(' ');
        for (auto item : td)
            bdata += QByteArray::fromHex(item.toLatin1());

        zigbee_protocol::ZigbeeFrame zf(bdata[2],bdata[3],(uint16_t)bdata[4] | bdata[5]<<8, bdata.data()+6,bdata.length()-7);

        return _zigbee->send(zf);
    }

    QByteArray bdata;
    if(_send_hex)
    {
        QStringList td = data.split(' ');
        for (auto item : td)
            bdata += QByteArray::fromHex(item.toLatin1());
    }
    else
        bdata = data.toUtf8();
    switch(_send_type)
    {
    case SendType::WithCarriageEnter:
        bdata += "\n";
        break;
    case SendType::WithLineFeed:
        bdata += "\r";
        break;
    case SendType::WithCarriageEnterAndLineFeed:
        bdata += "\r\n";
        break;
    case SendType::Blank:
    default:
        break;
    }
    return _serial_port.write(bdata);
}

void  SerialPortManager::zigbee_callback(zigbee_protocol::ZigbeeFrame zframe)
{
    QJsonObject object;
    QByteArray zf = QByteArray((char *)zframe.data(),zframe.size());
    QByteArray hex = zf.toHex(' ');
    QByteArray data = hex.toUpper();
    readyRead(false);
    object.insert("data_hex", QJsonValue(QString(data)));
    object.insert("type","zigbee_raw_data");
    _bus->push_data("zigbee_parser", object);
}

QSerialPort* SerialPortManager::get_serial()
{
    if (!_handledBymanager)
        return &_serial_port;
    else
        return nullptr;
}
