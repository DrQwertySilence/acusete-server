#include "data.h"

#include "application.h"
#include "database.h"

#include <QJsonObject>

/**
 * @brief Data::Data Constructor.
 * @param p_serialTimer Timer that represents the frecuenzy at what the application works.
 * @param p_Parent Parent QObject.
 */
Data::Data(QTimer* p_serialTimer, QObject *p_Parent) :
    QObject(p_Parent),
    m_sensorAlarm(new QSoundEffect(this)),
    m_timerAlarm(new QSoundEffect(this))
{
    // Alarm configuration
    m_sensorAlarm->setSource(QUrl::fromLocalFile(Configuration::configuration.getSoundConfiguration().value("strongAlarm").toString()));
    m_sensorAlarm->setLoopCount(QSoundEffect::Infinite);
    m_timerAlarm->setSource(QUrl::fromLocalFile(Configuration::configuration.getSoundConfiguration().value("softAlarm").toString()));
    m_timerAlarm->setLoopCount(QSoundEffect::Infinite);

    //Serial port and baud rate configuration
    QList<QJsonObject> deviceList = Configuration::configuration.getDevicesConfiguration();
    for (QJsonObject device : deviceList) {
        Device* dev = initDevices(device);
        m_devices.push_back(dev);
        QObject::connect(p_serialTimer, SIGNAL(timeout()),
                         dev, SLOT(getData()));
        QObject::connect(p_serialTimer, SIGNAL(timeout()),
                         dev, SLOT(checkData()));
        QObject::connect(dev, SIGNAL(dataChecked(int)),
                         (Application*)parent(), SLOT(startSensorAlarm(int)));
    }
}

/**
 * @brief Data::~Data Destroy the alarms, every device and timers that haven't finish.
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
 * @brief Data::initDevices Create device objects based on a configuration file content.
 * @param p_configuration The json object that containt the configuration.
 */
Device*
Data::initDevices(QJsonObject p_configuration)
{
    QSerialPort::BaudRate device_baud_rate;
    int baudRateInt = p_configuration.value("baudRate").toInt();
    switch (baudRateInt) {
    case 1200:
        device_baud_rate = QSerialPort::BaudRate::Baud1200;
        break;
    case 2400:
        device_baud_rate = QSerialPort::BaudRate::Baud2400;
        break;
    case 4800:
        device_baud_rate = QSerialPort::BaudRate::Baud4800;
        break;
    case 9600:
        device_baud_rate = QSerialPort::BaudRate::Baud9600;
        break;
    case 19200:
        device_baud_rate = QSerialPort::BaudRate::Baud19200;
        break;
    case 38400:
        device_baud_rate = QSerialPort::BaudRate::Baud38400;
        break;
    case 57600:
        device_baud_rate = QSerialPort::BaudRate::Baud57600;
        break;
    case 115200:
        device_baud_rate = QSerialPort::BaudRate::Baud115200;
        break;
    default:
        device_baud_rate = QSerialPort::BaudRate::UnknownBaud;
    }

    Device* dev = new Device(p_configuration.value("id").toString(),
                             p_configuration.value("port").toString(),
                             device_baud_rate,
                             p_configuration.value("ppm").toInt(),
                             p_configuration.value("temperature").toDouble());
    return dev;
}

/**
 * @brief Data::getDevices Getter.
 * @return Returns all the devices.
 */
QVector<Device*>
Data::getDevices()
{
    return m_devices;
}

/**
 * @brief Data::getTimerAlarm Getter.
 * @return Returns the timer alarm.
 */
QSoundEffect*
Data::getTimerAlarm()
{
    return m_timerAlarm;
}

/**
 * @brief Data::getSensorAlarm Getter.
 * @return Returns the sensor alarm.
 */
QSoundEffect*
Data::getSensorAlarm()
{
    return m_sensorAlarm;
}

/**
 * @brief Data::addTimer Adds a timer to the application.
 * @param p_milliseconds Cuantity of milliseconds until the timer expires.
 * @param p_description Description given to the timer.
 */
void
Data::addTimer(int p_milliseconds, QString p_description)
{
    Timer *timer = new Timer(p_milliseconds, p_description, this);
    m_timers.push_back(timer);

    connect(timer, &Timer::removed,
            this, &Data::removeTimer);
}

/**
 * @brief Data::getTimerById Getter.
 * @param p_id The id of the timer.
 * @return Returns the timer that match the id.
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
 * @brief Data::destroyTimerById Destroy a timer by id.
 * @param p_id The id of the timer to be destroyed.
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
 * @brief Data::getFormatedTimers Get all the timers as an QJsonArray.
 * @return Returns a list of all timers as a json array.
 */
QJsonArray
Data::getFormatedTimers()
{
    QJsonArray timers;
    for (auto timer : this->m_timers) {
        QJsonObject jsonTimer {
            {"ID", timer->getId()},
            {"description", timer->getDescription()},
            {"remainingTime", timer->getRemainingTime()}
        };
        timers.append(jsonTimer);
    }
    return timers;
}

/**
 * @brief Data::getFormatedDeviceData Get devices loaded into the applicatión.
 * @return Returns a list of devices as a QJsonArray.
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
 * @brief Device::getRecordedData Get all the records from the database from a period of time.
 * @param p_initialDate Initial date of a period of time as timestamp.
 * @param p_finalDate Final date of a period of time as timestamp.
 * @return Returns the recorded data as a QJsonArray.
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
 * @brief Data::removeTimer Remove a timer from the timer array.
 * @param p_timer The timer that is going to be removed.
 */
void
Data::removeTimer(Timer *p_timer)
{
    m_timers.removeOne(p_timer);
    delete p_timer;
}
