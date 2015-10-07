#include "data.h"

#include <stdlib.h>

#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

#include "application.h"

/**
 * @brief Data::Data
 * @param pParent
 */
Data::Data(QObject *pParent) :
    QObject(pParent),
    m_sensorAlarm(new QSound(m_configuration->getSensorAlarmPath().c_str())),
    m_timerAlarm(new QSound(m_configuration->getTimerAlarmPath().c_str()))
{
    /// Alarm configuration
    m_sensorAlarm->setLoops(-1);
    m_timerAlarm->setLoops(-1);

    ///Serial port and baud rate configuration
    initDevices(m_configuration->getDeviceListPath());
}

/**
 * @brief Data::~Data
 */
Data::~Data()
{
    delete m_sensorAlarm;
    delete m_timerAlarm;

    delete m_configuration;

    for (auto *device : m_devices)
        delete device;
    m_devices.clear();

    for (auto *timer : m_timers)
        delete timer;
    m_timers.clear();
}

/**
 * @brief Data::initDevices
 * @param p_path
 */
void
Data::initDevices(std::string p_path)
{
    std::vector<std::string> args = m_configuration->readFile(p_path);

    QSerialPort::BaudRate baudRate;
    int baudRateInt = atoi(args[1].c_str());
    switch (baudRateInt) {
    case 1200:
        baudRate = QSerialPort::BaudRate::Baud1200;
        break;
    case 2400:
        baudRate = QSerialPort::BaudRate::Baud2400;
        break;
    case 4800:
        baudRate = QSerialPort::BaudRate::Baud4800;
        break;
    case 9600:
        baudRate = QSerialPort::BaudRate::Baud9600;
        break;
    case 19200:
        baudRate = QSerialPort::BaudRate::Baud19200;
        break;
    case 38400:
        baudRate = QSerialPort::BaudRate::Baud38400;
        break;
    case 57600:
        baudRate = QSerialPort::BaudRate::Baud57600;
        break;
    case 115200:
        baudRate = QSerialPort::BaudRate::Baud115200;
        break;
    default:
        baudRate = QSerialPort::BaudRate::UnknownBaud;
    }

    m_devices.push_back(new Device(QString(args.at(0).c_str()), baudRate));
}

/**
 * @brief Data::getDevices
 * @return
 */
std::vector<Device*>
Data::getDevices()
{
    return m_devices;
}

/**
 * @brief Data::getTimerAlarm
 * @return
 */
QSound*
Data::getTimerAlarm()
{
    return m_timerAlarm;
}

/**
 * @brief Data::getSensorAlarm
 * @return
 */
QSound*
Data::getSensorAlarm()
{
    return m_sensorAlarm;
}

/**
 * @brief Data::addTimer
 * @param p_milliseconds
 */
void
Data::addTimer(int p_milliseconds)
{
    Timer *timer = new Timer(p_milliseconds, this);
    m_timers.push_back(timer);

    connect(timer, &Timer::removed,
            this, &Data::removeTimer);
}

/**
 * @brief Data::getTimers
 * @return
 */
QList<Timer *>
Data::getTimers()
{
    return m_timers;
}

/**
 * @brief Data::getTimerById
 * @param p_id
 * @return
 */
Timer*
Data::getTimerById(int p_id)
{
    for (Timer* timer : m_timers)
        if (timer->getId() == p_id)
            return timer;
    return nullptr;
}

/**
 * @brief Data::getFormatedTimers
 * @return
 */
std::string
Data::getFormatedTimers()
{
    std::string formatedTimers;

    for (auto timer : this->m_timers) {
        if (timer != (*m_timers.begin()))
            formatedTimers.append(" ");
        timer->getRemainingTime();

        int timerRT = timer->getRemainingTime();

        int hours = timerRT / (60 * 60 * 1000);
        int minutes = (timerRT - (hours * 60 * 60 * 1000)) / (60 * 1000);
        int seconds = (timerRT - (minutes * 60 * 1000) -  (hours * 60 * 60 * 1000)) / 1000;
//        int milliseconds = timerRT - (seconds * 1000) - (minutes * 1000 * 60) - (hours * 60 * 60 * 1000);

        std::string hoursStr = hours < 10 ? '0' + std::to_string(hours) : std::to_string(hours);
        std::string minutesStr = minutes < 10 ? '0' + std::to_string(minutes) : std::to_string(minutes);
        std::string secondsStr = seconds < 10 ? '0' + std::to_string(seconds) : std::to_string(seconds);

        std::string timerIdStr = std::to_string(timer->getId());

        formatedTimers.append(timerIdStr + ";");
        formatedTimers.append(hoursStr +
                              ":" +
                              minutesStr +
                              ":" +
                              secondsStr);/* +
                              ":" +
                              std::to_string(milliseconds));*/
    }

    return formatedTimers;
}

/**
 * @brief Data::getConfiguration
 * @return
 */
Configuration*
Data::getConfiguration() {
    return m_configuration;
}

/**
 * @brief Data::removeTimer
 * @param p_timer
 */
void
Data::removeTimer(Timer *p_timer)
{
    m_timers.removeOne(p_timer);
    delete p_timer;
}
