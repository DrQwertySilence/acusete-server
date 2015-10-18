#ifndef DATABASE
#define DATABASE

#include <time.h>

#include <QtSql>
#include <QtCore>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

struct RegisteredData
{
    int id;
    int date;
    int ppm;
    QVector<float> temperatures;
};

struct DeviceRecord
{
    QString deviceID;
    QVector<RegisteredData> registeredData;
};

const QString getSQLDriver();
const QString getDatabaseLocation();
const QString getDatabaseName();

QJsonObject getRegisteredDataByDevice_2(QString p_deviceID, int p_initialDate, int p_finalDate);

QVariant recordDeviceData(QSqlQuery &p_query, QString &p_deviceId, qint64 p_time, qint64 &p_ppm);
void recordTemperature(QSqlQuery &p_query, QVariant &p_registerId, float &p_temperature);
QSqlError recordData(QString p_deviceId, qint64 p_ppm, QVector<float> p_temperatures);

#endif // DATABASE
