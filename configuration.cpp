#include "configuration.h"

#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>

/**
 * @brief Configuration::Configuration
 */
Configuration::Configuration()
{

}

/**
 * @brief Configuration::getSensorAlarmPath
 * @return
 */
QString
Configuration::getSensorAlarmPath()
{
    return "../share/acusete/sound/alert-short.wav";
}

/**
 * @brief Configuration::getTimerAlarmPath
 * @return
 */
QString
Configuration::getTimerAlarmPath()
{
    return "../share/acusete/sound/alarm2.wav";
}

/**
 * @brief Configuration::getDeviceListPath
 * @return
 */
QString
Configuration::getDeviceListPath()
{
    return "../etc/acusete/arduino.json";
}

/**
 * @brief Configuration::readFile Get the content of a json file.
 * @param pPath The location of the file.
 * @return The content of the document as a json object.
 */
QJsonObject
Configuration::readFile(QString pPath)
{
    QFile file(pPath);
    if (!file.open(QIODevice::ReadOnly)) {
        ;
        return QJsonObject();
    } else {
        QJsonDocument config = QJsonDocument::fromJson(file.readAll());
        //
        file.close();
        //
        return config.object();
    }
}
