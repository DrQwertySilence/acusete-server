#include "configuration.h"

#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>

/**
 * @brief Configuration::configuration
 */
Configuration Configuration::configuration = Configuration("../etc/acusete/configuration.json");

/**
 * @brief Configuration::Configuration
 * @param p_configurationFilePath The path of the file that contains the file path of other configuration files.
 */
Configuration::Configuration(const QString p_configurationFilePath)
{
    QJsonObject obj = readFile(p_configurationFilePath);
    m_serverConfiguration = readFile(obj.value("server").toString());
    m_devicesConfiguration = readFile(obj.value("devices").toString());
    m_warningConfiguration = readFile(obj.value("warning").toString());
    m_soundConfiguration = readFile(obj.value("sound").toString());
}

/**
 * @brief Configuration::readFile Get the content of a json file.
 * @param pPath The location of the file.
 * @return The content of the document as a JSON object.
 */
QJsonObject
Configuration::readFile(const QString pPath)
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

/**
 * @brief Configuration::getServerConfiguration
 * @return
 */
QJsonObject
Configuration::getServerConfiguration()
{
    return m_serverConfiguration;
}

/**
 * @brief Configuration::getDevicesConfiguration
 * @return
 */
QJsonObject
Configuration::getDevicesConfiguration()
{
    return m_devicesConfiguration;
}

/**
 * @brief Configuration::getWarningConfiguration
 * @return
 */
QJsonObject
Configuration::getWarningConfiguration()
{
    return m_warningConfiguration;
}

/**
 * @brief Configuration::getSoundonfiguration
 * @return
 */
QJsonObject
Configuration::getSoundonConfiguration()
{
    return m_soundConfiguration;
}
