#ifndef DATABASE
#define DATABASE

#include <time.h>

#include <vector>

#include <QtSql>
#include <QtCore>

//struct SensorData
//{
//    int date;
//    int ppm;
//    std::vector<float> temperatures;
//};

struct DeviceRecord
{
    QString id;
    int date;
    int ppm;
    std::vector<float> temperatures;
};

const QString getSQLDriver()
{
    return "QSQLITE";
}

const QString getDatabaseLocation()
{
    return "../share/acusete/datalog.sqlite";
}

const QString getDatabaseName()
{
    return "record";
}

std::vector<DeviceRecord> getRecordData(int p_beginDate, int p_endDate)
{
    QSqlDatabase database = QSqlDatabase::addDatabase(getSQLDriver(), getDatabaseName());
    database.setDatabaseName(getDatabaseLocation());

    if (!database.open()) {
        return std::vector<DeviceRecord>();
    }
    std::vector<DeviceRecord> deviceRecords;

    QSqlQuery query = QSqlQuery(database);

    query.prepare("select * from datarecord where date >= ? and date <= ?");
    query.addBindValue(p_beginDate);
    query.addBindValue(p_endDate);
    if (query.exec()) {
        while (query.next()) {
            deviceRecords.push_back(DeviceRecord());
            deviceRecords.back().id = query.value(1).toString();
            deviceRecords.back().date = query.value(2).toInt();
            deviceRecords.back().ppm = query.value(3).toInt();
        }
    }

    query.prepare("select * from temperaturerecord where datarecordId = ?");
    for (DeviceRecord deviceRecord : deviceRecords) {
        query.addBindValue(deviceRecord.id);
        if (query.exec()) {
            while (query.next()) {
                deviceRecord.temperatures.push_back(query.value(2).toFloat());
            }
        }
    }

    database = QSqlDatabase();
    QSqlDatabase::removeDatabase(getDatabaseName());

    return deviceRecords;
}

QVariant recordDeviceData(QSqlQuery &p_query, QString &p_deviceId, qint64 p_time, qint64 &p_ppm)
{
//    time_t seconds;
//    seconds = time(NULL);

    p_query.addBindValue(p_deviceId);
    p_query.addBindValue(p_time);
    p_query.addBindValue(p_ppm);
    p_query.exec();
    return p_query.lastInsertId();
}

void recordTemperature(QSqlQuery &p_query, QVariant &p_registerId, float &p_temperature)
{
    p_query.addBindValue(p_registerId);
    p_query.addBindValue(p_temperature);
    p_query.exec();
}

QSqlError recordData(QString p_deviceId, qint64 p_ppm, std::vector<float> p_temperatures)
{
    QSqlDatabase database = QSqlDatabase::addDatabase(getSQLDriver(), getDatabaseName());
    database.setDatabaseName(getDatabaseLocation());

    if (!database.open())
        return database.lastError();

    QSqlQuery query = QSqlQuery(database);

    query.prepare("insert into datarecord(device, date, ppm) values(?, ?, ?)");
    QVariant registerId = recordDeviceData(query, p_deviceId, time(NULL), p_ppm);

    query.prepare("insert into temperaturerecord(datarecordId, temperature) values(?, ?)");
    for (auto temperature : p_temperatures) {
        recordTemperature(query, registerId, temperature);
    }

    database = QSqlDatabase();
    QSqlDatabase::removeDatabase(getDatabaseName());

    return QSqlError();
}

#endif // DATABASE
