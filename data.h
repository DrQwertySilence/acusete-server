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
    QList<Timer*> m_timers;
//    std::vector<Timer*> m_timers;

    void initDevices(std::string p_path);

private slots:
    void removeTimer(Timer *p_timer);

public:
    Data(QObject *pParant = nullptr);
    virtual ~Data();

    void addTimer(int p_milliseconds);

    std::vector<Device*> getDevices();
    QList<Timer *> getTimers();
    Timer* getTimerById(int p_id);

    QSound* getTimerAlarm();
    QSound* getSensorAlarm();

    std::string getFormatedTimers();

    Configuration* getConfiguration();
};

#endif // DATA_H
