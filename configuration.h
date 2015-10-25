
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QVector>

#include <QJsonObject>

class Configuration
{
private:
    QJsonObject m_serverConfiguration;
    QJsonObject m_devicesConfiguration;
    QJsonObject m_warningConfiguration;
    QJsonObject m_soundConfiguration;
    //
    QJsonObject readFile(const QString pPath);
    Configuration(const QString p_configurationFilePath);
public:
    static Configuration configuration;

    QJsonObject getServerConfiguration();
    QJsonObject getDevicesConfiguration();
    QJsonObject getWarningConfiguration();
    QJsonObject getSoundonConfiguration();
};

#endif // CONFIGURATION_H
