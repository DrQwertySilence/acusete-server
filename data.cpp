#include "data.h"

#include "application.h"
#include "database.h"

#include <QJsonObject>

/**
 * @brief Data::Data
 * @param pParent
 */
Data::Data(QObject *pParent) :
    QObject(pParent),
    m_sensorAlarm(new QSound(Configuration::configuration.getSoundonConfiguration().value("strongAlarm").toString())),
    m_timerAlarm(new QSound(Configuration::configuration.getSoundonConfiguration().value("softAlarm").toString()))
{
    /// Alarm configuration
    m_sensorAlarm->setLoops(-1);
    m_timerAlarm->setLoops(-1);

    ///Serial port and baud rate configuration
    initDevices(Configuration::configuration.getDevicesConfiguration());
}

/**
 * @brief Data::~Data
 */
Data::~Data()
{
    delete m_sensorAlarm;
    delete m_timerAlarm;

    for (QVector<Device*>::Iterator it = m_devices.begin(); it != m_devices.end(); ++it) {
        delete *it;
    }

    m_devices.clear();

    for (auto *timer : m_timers)
        delete timer;
    m_timers.clear();
}

/**
 * @brief Data::initDevices
 * @param p_configuration
 */
void
Data::initDevices(QJsonObject p_configuration)
{
    QSerialPort::BaudRate baudRate;
    int baudRateInt = p_configuration.value("baudRate").toInt();
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

    m_devices.push_back(new Device(p_configuration.value("port").toString(), baudRate));
}

/**
 * @brief Data::getDevices
 * @return
 */
QVector<Device*>
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
 * @brief Data::destroyTimerById
 * @param p_id
 */
void
Data::destroyTimerById(int p_id)
{
    const int lastElementPos = m_timers.size() - 1;
    const int count = m_timers.count();
    for (int i = 0; i < count; ++i)
        if (m_timers[i]->getId() == p_id) {
            Timer * timer = m_timers[i];
            m_timers.swap(i, lastElementPos);
            m_timers.pop_back();
            delete timer;
            break;
        }
}

/**
 * @brief Data::getFormatedTimers
 * @return
 */
QJsonArray
Data::getFormatedTimers()
{
    QJsonArray timers;
    for (auto timer : this->m_timers) {
        QJsonObject jsonTimer {
            {"ID", timer->getId()},
            {"remainingTime", (timer->getRemainingTime())}
        };
        timers.append(jsonTimer);
    }
    return timers;
}

/**
 * @brief Data::getFormatedDeviceData
 * @return
 */
QJsonArray
Data::getFormatedDeviceData()
{
    QJsonArray devicesData;
    for (Device *device : m_devices) {
        QJsonArray temperatures;

        for (auto temperature : device->getTemperatures()) {
            temperatures.append(temperature);
        }

        int seconds = time(NULL);
        QJsonObject jsonDevice {
            {"id", device->getId()},
            {"time", seconds},
            {"ppm", device->getPPM()},
            {"temperatures", temperatures}
        };

        devicesData.append(jsonDevice);
    }
    return devicesData;
}

/**
 * @brief Device::getRecordedData
 * @param p_initialDate
 * @param p_finalDate
 * @return
 */
QJsonArray
Data::getRecordedData(int p_initialDate, int p_finalDate)
{
    QJsonArray devicesRecord;
    for (Device *device : m_devices) {
        devicesRecord.append(getRegisteredDataByDevice(device->getId(), p_initialDate, p_finalDate));
    }
    return devicesRecord;
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
