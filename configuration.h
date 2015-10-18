#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QString>
#include <QVector>

#include <QJsonObject>

class Configuration
{
public:
    QString getSensorAlarmPath();
    QString getTimerAlarmPath();
    QString getDeviceListPath();

    QJsonObject readFile(QString pPath);

    Configuration();
};

#endif // CONFIGURATION_H
