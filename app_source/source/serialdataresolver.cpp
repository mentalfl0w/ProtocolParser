#include "serialdataresolver.h"
#include "protocol.h"
#include <QThread>

SerialDataResolver::SerialDataResolver(QObject *parent)
    : QObject{parent}
{
    //qDebug()<<__FUNCTION__<<"Thread: "<<QThread::currentThread();
    _serial = new QSerialPort(this);
    _zigbee = zigbee_protocol::DLLN3X::instance();
    recv_hex(false);
    handled_by_manager(true);
    connect(this, &SerialDataResolver::handled_by_managerChanged, this, [=](){
        if (!_handled_by_manager && _serial->isOpen())
        {
            _zigbee->init(_serial, _serial->baudRate());
            zigbee_protocol::Protocol::getInstance()->self_addr = _zigbee->read_addr();
            connect(_zigbee, &zigbee_protocol::DLLN3X::recved, this, &SerialDataResolver::zigbee_callback);
        }
        else
        {
            disconnect(_zigbee, &zigbee_protocol::DLLN3X::recved, this, &SerialDataResolver::zigbee_callback);
        }
    });
}

SerialDataResolver::~SerialDataResolver()
{
    close();
    delete _serial;
}

SerialDataResolver* SerialDataResolver::instance()
{
    //qDebug()<<__FUNCTION__<<"Thread: "<<QThread::currentThread();
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    static SerialDataResolver *singleton = nullptr;
    if (!singleton) {
        singleton = new SerialDataResolver();
    }
    return singleton;
}

void SerialDataResolver::open(QString port_name, QString baudrate, QString databits,
                              QString parity, QString stopbits)
{
    //qDebug()<<__FUNCTION__<<"Thread: "<<QThread::currentThread();
    QMutex mutex;
    const QMutexLocker locker(&mutex);
    _serial->setPortName(port_name);

    if (!_serial->open(QIODevice::ReadWrite))
    {
        emit error(tr("串口%1打开失败, 错误代码：%2")
                       .arg(port_name).arg(_serial->error()));
        return;
    }
    else
    {
        serial_opened(true);
        _serial->setBaudRate(baudrate.toInt());
        switch (databits.toInt())
        {
        case 5:
            _serial->setDataBits( QSerialPort::Data5 ); // 设置数据位(数据位为5位)
            break;
        case 6:
            _serial->setDataBits( QSerialPort::Data6 ); // 设置数据位(数据位为6位)
            break;
        case 7:
            _serial->setDataBits( QSerialPort::Data7 ); // 设置数据位(数据位为7位)
            break;
        case 8:
            _serial->setDataBits( QSerialPort::Data8 ); // 设置数据位(数据位为8位)
            break;
        default:
            break;
        }
        if (parity=="NONE")
        {
            _serial->setParity( QSerialPort::NoParity ); // 设置校验位(无校验位)
        }
        else if (parity=="EVEN")
        {
            _serial->setParity( QSerialPort::EvenParity ); // 设置校验位(奇校验位)
        }
        else
        {
            _serial->setParity( QSerialPort::OddParity ); // 设置校验位(偶校验位)
        }
        if (stopbits=="1")
        {
            _serial->setStopBits( QSerialPort::OneStop ); // 设置停止位(停止位为1)
        }
        else if (stopbits=="1.5")
        {
            _serial->setStopBits( QSerialPort::OneAndHalfStop ); // 设置停止位(停止位为1.5)
        }
        else
        {
            _serial->setStopBits( QSerialPort::TwoStop ); // 设置停止位(停止位为2)
        }
        _serial->setFlowControl( QSerialPort::NoFlowControl ); // 设置流控制(无数据流控制)
        connect(_serial,&QSerialPort::aboutToClose, this, &SerialDataResolver::close);
        connect(_serial,&QSerialPort::readyRead, this, &SerialDataResolver::readHandler);
        if (!_handled_by_manager)
        {
            _zigbee->init(_serial, _serial->baudRate());
            zigbee_protocol::Protocol::getInstance()->self_addr = _zigbee->read_addr();
            connect(_zigbee, &zigbee_protocol::DLLN3X::recved, this, &SerialDataResolver::zigbee_callback);
        }
    }
}

bool SerialDataResolver::handled_by_manager()
{
    return _handled_by_manager;
}

void SerialDataResolver::handled_by_manager(bool handled_by_manager)
{
    QMutex mutex;
    const QMutexLocker locker(&mutex);
    _handled_by_manager = handled_by_manager;
    emit handled_by_managerChanged();
}

bool SerialDataResolver::recv_hex()
{
    return _recv_hex;
}

void SerialDataResolver::recv_hex(bool recv_hex)
{
    QMutex mutex;
    const QMutexLocker locker(&mutex);
    _recv_hex = recv_hex;
    emit recv_hexChanged();
}

bool SerialDataResolver::serial_opened()
{
    return _serial_opened;
}

void SerialDataResolver::serial_opened(bool serial_opened)
{
    QMutex mutex;
    const QMutexLocker locker(&mutex);
    _serial_opened = serial_opened;
    emit serial_openedChanged();
}

void SerialDataResolver::readHandler()
{
    //qDebug()<<__FUNCTION__<<"Thread: "<<QThread::currentThread();
    if (_handled_by_manager)
    {
        QByteArray bdata;
        if (_serial->isOpen() && _handled_by_manager)
        {
            while(true){
                bdata += _serial->readAll();
                if (!_serial->waitForReadyRead(20))
                    break;
            }
            if (_recv_hex)
            {
                emit recved(bdata.toHex(' ').toUpper());
            }
            else
                emit recved(bdata);
        }
        else
            emit recved(bdata);
    }
    else
        _zigbee->loop();
}

void SerialDataResolver::close()
{
    //qDebug()<<__FUNCTION__<<"Thread: "<<QThread::currentThread();
    serial_opened(false);
    disconnect(_serial,&QSerialPort::aboutToClose,this,&SerialDataResolver::close);
    disconnect(_zigbee, &zigbee_protocol::DLLN3X::recved, this, &SerialDataResolver::zigbee_callback);
    if(_serial->isOpen())
    {
        _serial->clear();
        _serial->close();
    }
}

void SerialDataResolver::write_data(QString data, SendType type, bool send_hex)
{
    //qDebug()<<__FUNCTION__<<"Thread: "<<QThread::currentThread();
    QByteArray bdata;
    if (!_serial->isOpen())
        return;

    if (!_handled_by_manager)
    {

        QStringList td = data.split(' ');
        for (auto item : td)
            bdata += QByteArray::fromHex(item.toLatin1());
        zigbee_protocol::ZigbeeFrame zf;
        zf.load_package((uint8_t*)bdata.data(),bdata.length());
        _zigbee->send(zf);
        return;
    }

    if(send_hex)
    {
        QStringList td = data.split(' ');
        for (auto item : td)
            bdata += QByteArray::fromHex(item.toLatin1());
    }
    else
        bdata = data.toUtf8();
    switch(type)
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
    _serial->write(bdata);
    _serial->flush();
    return;
}

void SerialDataResolver::zigbee_callback(zigbee_protocol::ZigbeeFrame zframe)
{
    emit recved_zframe(zframe);
}
