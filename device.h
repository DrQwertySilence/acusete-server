#ifndef DEVICE_H
#define DEVICE_H

#include <vector>
#include <string>

#include <QObject>

#include <QSerialPort>

#include <QSqlQuery>

class Device : public QObject
{
    Q_OBJECT
private:
    QSerialPort *m_serialPort;

    std::string m_id;

    QByteArray *m_dataByteArray; // Where data from serial is temporaly stored
    int m_maxArraySize; // Max size of the buffer

    int m_ppm;
    std::vector<float> m_temperatures;

signals:
    void dataReceived();

public slots:
    void getData();

public:
    Device(QString pPort, QSerialPort::BaudRate pBaudRate);
    virtual ~Device();

    int getPPM();
    std::vector<float> getTemperatures();

    std::string getId();

    std::string getRecordedData(int p_initialDate, int p_finalDate);

    // SQL stuff
    void recordAllData();
};

#endif // DEVICE_H
