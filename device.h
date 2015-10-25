#ifndef DEVICE_H
#define DEVICE_H

#include <QObject>
#include <QSerialPort>
#include <QSqlQuery>
#include <QVector>

class Device : public QObject
{
    Q_OBJECT
private:
    QSerialPort *m_serialPort;
    QString m_id;
    QByteArray *m_dataByteArray; // Where data from serial is temporaly stored
    int m_maxArraySize; // Max size of the buffer
    int m_ppm;
    QVector<float> m_temperatures;

signals:
    void dataReceived();

public slots:
    void getData();

public:
    Device(QString pPort, QSerialPort::BaudRate pBaudRate);
    virtual ~Device();

    int getPPM();
    QVector<float> getTemperatures();
    QString getId();
    // SQL stuff
    void recordAllData();
};

#endif // DEVICE_H
