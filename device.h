#ifndef DEVICE_H
#define DEVICE_H

#include <vector>

#include <QSerialPort>

class Device
{
private:
    QSerialPort *mSerialPort;
    QString mPort;
    QSerialPort::BaudRate mBaudRate;
    std::vector<int> mTempSensors;
public:
    Device(QString pPort, QSerialPort::BaudRate pBaudRate);
    virtual ~Device();
    QSerialPort* getSerialPort();
    int configure();
};

#endif // DEVICE_H
