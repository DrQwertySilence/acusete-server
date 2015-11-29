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
    QByteArray *m_dataByteArray;
    int m_maxArraySize;

    int m_ppm;
    int m_ppmMax;

    QVector<float> m_temperatures;
    double m_temperatureMin;

signals:
    void dataReceived();
    void dataChecked(int p_state);

public slots:
    void getData();
    void checkData();

public:
    Device(QString p_id, QString p_port, QSerialPort::BaudRate p_baudRate, int p_ppmMax, double p_temperatureMin);
    virtual ~Device();

    int getPPM();
    QVector<float> getTemperatures();
    QString getId();
    void recordAllData();
};

#endif // DEVICE_H
