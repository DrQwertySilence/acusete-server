#include "configuration.h"

#include <QFile>

#include <QJsonDocument>
#include <QJsonObject>

#include <QDirIterator>

/**
 * @brief Configuration::configuration Singleton that containt all the configuration of the application.
 */
Configuration Configuration::configuration = Configuration("../etc/acusete/configuration.json");

/**
 * @brief Configuration::Configuration Parses a json file to get the configuration of the application.
 * @param p_configurationFilePath The path of the file that contains the file path of other configuration files.
 */
Configuration::Configuration(const QString p_configurationFilePath)
{
    QJsonObject obj = readFile(p_configurationFilePath);
    m_serverConfiguration = readFile(obj.value("server").toString());
    m_warningConfiguration = readFile(obj.value("warning").toString());
    m_soundConfiguration = readFile(obj.value("sound").toString());

    QDir dir(obj.value("devices").toString());
    QStringList filePathList = dir.entryList(QStringList() << "*.conf");

    for (QString filePath : filePathList)
        m_devicesConfiguration.append(readFile(dir.filePath(filePath)));
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
        return QJsonObject();
    } else {
        QJsonDocument config = QJsonDocument::fromJson(file.readAll());
        file.close();
        return config.object();
    }
}

/**
 * @brief Configuration::getServerConfiguration Getter.
 * @return Return the configuration of the websocket server (port number).
 */
QJsonObject
Configuration::getServerConfiguration()
{
    return m_serverConfiguration;
}

/**
 * @brief Configuration::getDevicesConfiguration Getter.
 * @return Returns the configuration of each device (represented as json object).
 */
QList<QJsonObject>
Configuration::getDevicesConfiguration()
{
    return m_devicesConfiguration;
}

/**
 * @brief Configuration::getWarningConfiguration Getter.
 * @return Return the configuration of when the (strong) alarm should start.
 */
QJsonObject
Configuration::getWarningConfiguration()
{
    return m_warningConfiguration;
}

/**
 * @brief Configuration::getSoundConfiguration Getter.
 * @return Return the configuration of where are the sound files (resources) used by alarms.
 */
QJsonObject
Configuration::getSoundConfiguration()
{
    return m_soundConfiguration;
}
