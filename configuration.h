
#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QVector>

#include <QJsonObject>

class Configuration
{
private:
    QJsonObject m_serverConfiguration;
    QJsonObject m_devicesConfiguration_OLD;
    QJsonObject m_warningConfiguration;
    QJsonObject m_soundConfiguration;

    QList<QJsonObject> m_devicesConfiguration;
    //
    QJsonObject readFile(const QString pPath);
    Configuration(const QString p_configurationFilePath);
public:
    static Configuration configuration;

    QJsonObject getServerConfiguration();
    QJsonObject getWarningConfiguration();
    QJsonObject getSoundConfiguration();
    QList<QJsonObject> getDevicesConfiguration();
};

#endif // CONFIGURATION_H
