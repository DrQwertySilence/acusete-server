#include "device.h"

Device::Device(QString pPort, QSerialPort::BaudRate pBaudRate) : mPort(pPort), mBaudRate(pBaudRate), mSerialPort(new QSerialPort())
{
    mSerialPort->setPortName(mPort);
    mSerialPort->setBaudRate(mBaudRate);

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

int
Device::configure()
{
    return 0;
}
