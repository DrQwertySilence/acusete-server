#include "timer.h"

#include <iostream>

#include "application.h"

Timer::Timer(int pTime, QObject *pParent) :
    QTimer(pParent)
{
    connect(this, &Timer::timeout,
            (Application*)this->parent()->parent(), &Application::startTimerAlarm);
    connect(this, &Timer::timeout,
            this, &Timer::ontimeout);

    this->setSingleShot(true);
    this->start(pTime);
}

Timer::~Timer()
{

}

void
Timer::ontimeout()
{
    remove(this);
}
