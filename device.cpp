#include "device.h"

Device::Device(QString pPort, QSerialPort::BaudRate pBaudRate) :
    mBaudRate(pBaudRate),
    mPort(pPort),
    mSerialPort(new QSerialPort())
{
    mSerialPort->setBaudRate(mBaudRate);
    mSerialPort->setPortName(mPort);

    mSerialPort->setDataBits(QSerialPort::DataBits::Data7);
    mSerialPort->setParity(QSerialPort::Parity::EvenParity);
    mSerialPort->setStopBits(QSerialPort::StopBits::OneStop);
    mSerialPort->setFlowControl(QSerialPort::FlowControl::NoFlowControl);

    mSerialPort->open(QIODevice::ReadOnly);
}

Device::~Device()
{
    delete mSerialPort;
}

QSerialPort*
Device::getSerialPort()
{
    return mSerialPort;
}
