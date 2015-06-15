#include "timer.h"

#include <iostream>

#include "application.h"

Timer::Timer(int pTime, QObject *pParent):
    QTimer(pParent)
{
    connect(this, &Timer::timeout,
            (Application*)this->parent()->parent(), &Application::startAlert);

    this->setSingleShot(true);

    std::cout << pTime << 'n';

    this->start(pTime);
}

Timer::~Timer()
{

}