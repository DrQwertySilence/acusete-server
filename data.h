#ifndef DATA_H
#define DATA_H

#include <vector>
#include <string>
#include <vector>

#include <QObject>
#include <QSound>

#include "timer.h"
#include "device.h"
#include "server.h"

class Data : public QObject
{
    Q_OBJECT
private:
    QSound *mAlarm;
    Server *mServer;
    std::vector<Device*> mDevices;
    std::vector<Timer*> mTimers;
public:
    Data(QObject *pParant = nullptr);
    virtual ~Data();

    void addTimer(int pMilliseconds);

    Server* getServer();
    std::vector<Device*> getDevices();
    std::vector<Timer *> getTimers();
    QSound* getAlarm();

    std::string getFormatedTimers();
};

#endif // DATA_H
