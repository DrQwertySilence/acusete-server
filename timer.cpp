#include "timer.h"

#include "application.h"

/**
 * @brief Timer::idCount The id that is going to be assigned to a new Timer object.
 */
int Timer::idCount = 0;

/**
 * @brief Timer::getNextId Beware, this definition can create timers with a same id.
 * @return An id, derp.
 */
int
Timer::getNextId()
{
    ++idCount;
    return idCount;
}

/**
 * @brief Timer::Timer Works as a wrapper for QTimer class.
 * @param p_time Amount of time until the timer finish
 * @param p_parent QObject parent.
 */
Timer::Timer(int p_time, QString p_description, QObject *p_parent) :
    QObject(p_parent),
    m_id(getNextId()),
    m_description(p_description),
    m_initialTime(p_time),
    m_remainingTime(p_time),
    m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout,
            (Application*)this->parent()->parent(), &Application::startTimerAlarm);
    connect(m_timer, &QTimer::timeout,
            this, &Timer::ontimeout);

    m_timer->setSingleShot(true);
    m_timer->start(p_time);
}

/**
 * @brief Timer::~Timer Destroy the QTimer object.
 */
Timer::~Timer()
{
    delete m_timer;
    m_timer = nullptr;
}

/**
 * @brief Timer::resume Start the QTimer object inside this object.
 */
void
Timer::resume()
{
    if (m_timer == nullptr)
        m_timer = new QTimer();
    m_timer->start(m_remainingTime);
    //
    m_state = TIMER_STATE::COUNTING;
    //
    resumed();
}

/**
 * @brief Timer::pause Pause the QTimer object inside this object.
 */
void
Timer::pause()
{
    if (m_timer->timerId() != -1) {
        m_remainingTime = m_timer->remainingTime();
        m_timer->stop();
    }
    //
    m_state = TIMER_STATE::PAUSED;
    //
    paused();
}

/**
 * @brief Timer::stop Stop the QTimer object inside this object.
 */
void
Timer::stop()
{
//    m_remainingTime = -1;
    int i = m_timer->remainingTime();
    if (i != -1)
        m_remainingTime = i;

    m_timer->stop();
    //
    m_state = TIMER_STATE::STOPPED;
    //
    stopped();
}

/**
 * @brief Timer::restart Start the QTimer object inside this object with the original time given at the start of this object creation.
 */
void
Timer::restart()
{
    delete m_timer;
    m_timer = new QTimer();
    m_timer->start(m_initialTime);
    //
    m_state = TIMER_STATE::COUNTING;
    //
    restarted();
}

/**
 * @brief Timer::destroy Stop the Qtimer object inside this object.
 */
void
Timer::destroy()
{
    destroyed();
    //
    m_state = TIMER_STATE::DESTROYED;
    //
    ((Data *)parent())->destroyTimerById(m_id);
//    delete this;
}

/**
 * @brief Timer::getId Getter.
 * @return The id ob this object, this id is not equal to the QTimer object id.
 */
int
Timer::getId()
{
    return m_id;
}

/**
 * @brief Timer::getDescription
 * @return
 */
QString
Timer::getDescription()
{
    return m_description;
}

/**
 * @brief Timer::getRemainingTime Getter
 * @return Returns the remaining time of the QTimer object inside this object.
 */
int
Timer::getRemainingTime()
{
    if (m_timer->timerId() == -1)
        return m_remainingTime;
    else
        return m_timer->remainingTime();
}

/**
 * @brief Timer::ontimeout
 */
void
Timer::ontimeout()
{
}
