#include "device.h"

Device::Device(QString pPort, QSerialPort::BaudRate pBaudRate) :
    m_baudRate(pBaudRate),
    m_port(pPort),
    m_serialPort(new QSerialPort())
{
    m_serialPort->setBaudRate(m_baudRate);
    m_serialPort->setPortName(m_port);

    m_serialPort->setDataBits(QSerialPort::DataBits::Data7);
    m_serialPort->setParity(QSerialPort::Parity::EvenParity);
    m_serialPort->setStopBits(QSerialPort::StopBits::OneStop);
    m_serialPort->setFlowControl(QSerialPort::FlowControl::NoFlowControl);

    m_serialPort->open(QIODevice::ReadOnly);
}

Device::~Device()
{
    delete m_serialPort;
}

QSerialPort*
Device::getSerialPort()
{
    return m_serialPort;
}
