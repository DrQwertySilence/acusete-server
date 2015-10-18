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
    m_sensorAlarm(new QSound(m_configuration->getSensorAlarmPath().toUtf8().data())),
    m_timerAlarm(new QSound(m_configuration->getTimerAlarmPath().toUtf8().data()))
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

    for (QVector<Device*>::Iterator it = m_devices.begin(); it != m_devices.end(); ++it) {
        delete *it;
    }

//    for (auto *device : m_devices)
//        delete device;
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
Data::initDevices(QString p_path)
{
    QJsonObject args = m_configuration->readFile(p_path);

    QSerialPort::BaudRate baudRate;
    int baudRateInt = args.value("baudRate").toInt();
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

    m_devices.push_back(new Device(args.value("port").toString(), baudRate));
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
 * @brief Data::destroyTimerById
 * @param p_id
 */
void
Data::destroyTimerById(int p_id)
{
    int count = m_timers.count();
    for (int i = 0; i < count; ++i)
        if (m_timers[i]->getId() == p_id) {
            delete m_timers[i];
            m_timers.removeAt(i);
        }
}

///**
// * @brief Data::getFormatedTimers
// * @return
// */
//std::string
//Data::getFormatedTimers()
//{
//    std::string formatedTimers;

//    for (auto timer : this->m_timers) {
//        if (timer != (*m_timers.begin()))
//            formatedTimers.append(" ");
//        timer->getRemainingTime();

//        int timerRT = timer->getRemainingTime();

//        int hours = timerRT / (60 * 60 * 1000);
//        int minutes = (timerRT - (hours * 60 * 60 * 1000)) / (60 * 1000);
//        int seconds = (timerRT - (minutes * 60 * 1000) -  (hours * 60 * 60 * 1000)) / 1000;
////        int milliseconds = timerRT - (seconds * 1000) - (minutes * 1000 * 60) - (hours * 60 * 60 * 1000);

//        std::string hoursStr = hours < 10 ? '0' + std::to_string(hours) : std::to_string(hours);
//        std::string minutesStr = minutes < 10 ? '0' + std::to_string(minutes) : std::to_string(minutes);
//        std::string secondsStr = seconds < 10 ? '0' + std::to_string(seconds) : std::to_string(seconds);

//        std::string timerIdStr = std::to_string(timer->getId());

//        formatedTimers.append(timerIdStr + ";");
//        formatedTimers.append(hoursStr +
//                              ":" +
//                              minutesStr +
//                              ":" +
//                              secondsStr);/* +
//                              ":" +
//                              std::to_string(milliseconds));*/
//    }

//    return formatedTimers;
//}

QJsonArray
Data::getFormatedTimers()
{
    QJsonArray timers;
    for (auto timer : this->m_timers) {
        QJsonObject jsonTimer {
            {"ID", timer->getId()},
            {"remainingTime", (timer->getRemainingTime() / 1000)}
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
        devicesRecord.append(getRegisteredDataByDevice_2(device->getId(), p_initialDate, p_finalDate));
    }
    return devicesRecord;
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
