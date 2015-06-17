#ifndef DEVICE_H
#define DEVICE_H

#include <vector>

#include <QSerialPort>

class Device
{
private:
    std::vector<int> mTempSensors;
    QString mPort;
    QSerialPort *mSerialPort;
    QSerialPort::BaudRate mBaudRate;

public:
    Device(QString pPort, QSerialPort::BaudRate pBaudRate);
    virtual ~Device();
    QSerialPort* getSerialPort();
};

#endif // DEVICE_H
