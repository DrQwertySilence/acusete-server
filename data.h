#ifndef DATA_H
#define DATA_H

#include <vector>
#include <string>
#include <vector>

#include <QObject>
#include <QSound>

#include "timer.h"
#include "device.h"
#include "configuration.h"

class Data : public QObject
{
    Q_OBJECT
private:
    QSound *m_sensorAlarm;
    QSound *m_timerAlarm;

    Configuration *m_configuration;
    std::vector<Device*> m_devices;
    std::vector<Timer*> m_timers;

public:
    Data(QObject *pParant = nullptr);
    virtual ~Data();

    void addTimer(int pMilliseconds);

    std::vector<Device*> getDevices();
    std::vector<Timer *> getTimers();
    QSound* getTimerAlarm();
    QSound* getSensorAlarm();

    std::string getFormatedTimers();

    Configuration* getConfiguration();
};

#endif // DATA_H
