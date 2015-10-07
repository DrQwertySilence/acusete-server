#include "timer.h"

#include <iostream>

#include "application.h"

/**
 * @brief Timer::idCount
 */
int Timer::idCount = 0;

/**
 * @brief Timer::getNextId Beware, this definition can create timers with a same id.
 * @return An id, derp.
 */
int
Timer::getNextId()
{
    idCount++;
    return idCount;
}

/**
 * @brief Timer::Timer
 * @param p_time
 * @param p_parent
 */
Timer::Timer(int p_time, QObject *p_parent) :
    QObject(p_parent),
    m_id(getNextId()),
    m_initialTime(p_time),
    m_remainingTime(p_time),
    m_timer(new QTimer)
{
    connect(m_timer, &QTimer::timeout,
            (Application*)this->parent()->parent(), &Application::startTimerAlarm);
    connect(m_timer, &QTimer::timeout,
            this, &Timer::ontimeout);

    m_timer->setSingleShot(true);
    m_timer->start(p_time);
}

/**
 * @brief Timer::~Timer
 */
Timer::~Timer()
{
    delete m_timer;
    m_timer = nullptr;
}

/**
 * @brief Timer::play
 */
void
Timer::resume()
{
    if (m_timer == nullptr)
        m_timer = new QTimer();
    m_timer->start(m_remainingTime);
}

/**
 * @brief Timer::pause
 */
void
Timer::pause()
{
    m_remainingTime = m_timer->remainingTime();
    m_timer->stop();
    delete m_timer;
}

/**
 * @brief Timer::remove
 */
void
Timer::stop()
{
    m_remainingTime = m_timer->remainingTime();
    delete this;
}

/**
 * @brief Timer::restart
 */
void
Timer::restart()
{
    delete m_timer;
    m_timer = new QTimer();
    m_timer->start(m_initialTime);
}

/**
 * @brief Timer::getId
 * @return
 */
int
Timer::getId()
{
    return m_id;
}

int
Timer::getInitialTime()
{
    return m_initialTime;
}

/**
 * @brief Timer::getRemainingTime
 * @return
 */
int
Timer::getRemainingTime()
{
    return m_timer->remainingTime();
}

/**
 * @brief Timer::ontimeout
 */
void
Timer::ontimeout()
{
}
