#ifndef TIMER_H
#define TIMER_H

#include <QTimer>

#include "device.h"

class Timer: public QTimer
{
private:
//    int mTime;
    Device *mDevice;
public:
    Timer(int pTime, QObject *pParent = nullptr);
    virtual ~Timer();
};

#endif // TIMER_H
