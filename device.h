#ifndef DEVICE_H
#define DEVICE_H

#include <vector>

#include <QSerialPort>

class Device
{
private:
    std::vector<int> m_tempSensors;
    QString m_port;
    QSerialPort *m_serialPort;
    QSerialPort::BaudRate m_baudRate;

public:
    Device(QString pPort, QSerialPort::BaudRate pBaudRate);
    virtual ~Device();
    QSerialPort* getSerialPort();
};

#endif // DEVICE_H
