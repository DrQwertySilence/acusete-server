#ifndef DATABASE
#define DATABASE

#include <time.h>

#include <vector>

#include <QtSql>
#include <QtCore>

struct RegisteredData
{
    int id;
    int date;
    int ppm;
    std::vector<float> temperatures;
};

struct DeviceRecord
{
    std::string deviceID;
    std::vector<RegisteredData> registeredData;
};

const QString getSQLDriver();
const QString getDatabaseLocation();
const QString getDatabaseName();
DeviceRecord getRegisteredDataByDevice(std::string p_deviceID, int p_initialDate, int p_finalDate);
QVariant recordDeviceData(QSqlQuery &p_query, QString &p_deviceId, qint64 p_time, qint64 &p_ppm);
void recordTemperature(QSqlQuery &p_query, QVariant &p_registerId, float &p_temperature);
QSqlError recordData(QString p_deviceId, qint64 p_ppm, std::vector<float> p_temperatures);

#endif // DATABASE
