#ifndef TIMER_H
#define TIMER_H

#include <QTimer>
#include <QObject>

#include "device.h"

class Timer: public QTimer
{
    Q_OBJECT
public:
    Timer(int pTime, QObject *pParent = nullptr);
    virtual ~Timer();
private slots:
    void ontimeout();
signals:
    void remove(Timer *p_timer);
};

#endif // TIMER_H
