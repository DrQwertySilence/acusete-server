#ifndef DATA_H
#define DATA_H

#include "timer.h"

#include <QObject>
#include <QSound>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QVector>
//test
#include <QLinkedList>

#include "device.h"
#include "configuration.h"

class Data : public QObject
{
    Q_OBJECT
private:
    QSound *m_sensorAlarm;
    QSound *m_timerAlarm;
    QVector<Device*> m_devices;
    QList<Timer*> m_timers;
    void initDevices(QJsonObject p_configuration);

private slots:
    void removeTimer(Timer *p_timer);

public:
    Data(QObject *pParant = nullptr);
    virtual ~Data();

    void addTimer(int p_milliseconds);
    QVector<Device*> getDevices();
    Timer* getTimerById(int p_id);
    void destroyTimerById(int p_id);
    QSound* getTimerAlarm();
    QSound* getSensorAlarm();
    QJsonArray getFormatedTimers();
    QJsonArray getFormatedDeviceData();
    QJsonArray getRecordedData(int p_initialDate, int p_finalDate);
};

#endif // DATA_H
