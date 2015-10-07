#include "database.h"

/**
 * @brief getSQLDriver
 * @return
 */
const QString getSQLDriver()
{
    return "QSQLITE";
}

/**
 * @brief getDatabaseLocation
 * @return
 */
const QString getDatabaseLocation()
{
    return "../share/acusete/datalog.sqlite";
}

/**
 * @brief getDatabaseName
 * @return
 */
const QString getDatabaseName()
{
    return "record";
}

/**
 * @brief getRegisteredDataByDevice
 * @param p_deviceID
 * @param p_initialDate
 * @param p_finalDate
 * @return
 */
DeviceRecord getRegisteredDataByDevice(std::string p_deviceID, int p_initialDate, int p_finalDate)
{
    QSqlDatabase database = QSqlDatabase::addDatabase(getSQLDriver(), getDatabaseName());
    database.setDatabaseName(getDatabaseLocation());

    if (!database.open()) {
        return DeviceRecord();
    }
    DeviceRecord deviceRecords;
    deviceRecords.deviceID = p_deviceID;

    QSqlQuery query = QSqlQuery(database);

    query.prepare("select * from datarecord where device = ? and date >= ? and date <= ? and rowid % 10 = 0 order by date");
    query.addBindValue(QString(p_deviceID.c_str()));
    query.addBindValue(p_initialDate);
    query.addBindValue(p_finalDate);
    if (query.exec()) {
        while (query.next()) {
            deviceRecords.registeredData.push_back(RegisteredData());
            deviceRecords.registeredData.back().id = query.value(0).toInt();
            deviceRecords.registeredData.back().date = query.value(2).toInt();
            deviceRecords.registeredData.back().ppm = query.value(3).toInt();
        }
    }

    query.prepare("select * from temperaturerecord where datarecordId = ?");
    for (RegisteredData &registeredData : deviceRecords.registeredData) {
        query.addBindValue(registeredData.id);
        if (query.exec()) {
            while (query.next()) {
                float temperature = query.value(2).toFloat();
                registeredData.temperatures.push_back(temperature);
            }
        }
    }

    database = QSqlDatabase();
    QSqlDatabase::removeDatabase(getDatabaseName());

    return deviceRecords;
}

//std::vector<DeviceRecord> getRecordData(int p_beginDate, int p_endDate)
//{
//    QSqlDatabase database = QSqlDatabase::addDatabase(getSQLDriver(), getDatabaseName());
//    database.setDatabaseName(getDatabaseLocation());

//    if (!database.open()) {
//        return std::vector<DeviceRecord>();
//    }
//    std::vector<DeviceRecord> deviceRecords;

//    QSqlQuery query = QSqlQuery(database);

//    query.prepare("select * from datarecord where date >= ? and date <= ?");
//    query.addBindValue(p_beginDate);
//    query.addBindValue(p_endDate);
//    if (query.exec()) {
//        while (query.next()) {
//            deviceRecords.push_back(DeviceRecord());
//            deviceRecords.back().deviceID = query.value(1).toString();
//            deviceRecords.back().date = query.value(2).toInt();
//            deviceRecords.back().ppm = query.value(3).toInt();
//        }
//    }

//    query.prepare("select * from temperaturerecord where datarecordId = ?");
//    for (DeviceRecord deviceRecord : deviceRecords) {
//        query.addBindValue(deviceRecord.deviceID);
//        if (query.exec()) {
//            while (query.next()) {
//                deviceRecord.temperatures.push_back(query.value(2).toFloat());
//            }
//        }
//    }

//    database = QSqlDatabase();
//    QSqlDatabase::removeDatabase(getDatabaseName());

//    return deviceRecords;
//}

/**
 * @brief recordDeviceData
 * @param p_query
 * @param p_deviceId
 * @param p_time
 * @param p_ppm
 * @return
 */
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

/**
 * @brief recordTemperature
 * @param p_query
 * @param p_registerId
 * @param p_temperature
 */
void recordTemperature(QSqlQuery &p_query, QVariant &p_registerId, float &p_temperature)
{
    p_query.addBindValue(p_registerId);
    p_query.addBindValue(p_temperature);
    p_query.exec();
}

/**
 * @brief recordData
 * @param p_deviceId
 * @param p_ppm
 * @param p_temperatures
 * @return
 */
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
    for (auto &temperature : p_temperatures) {
        recordTemperature(query, registerId, temperature);
    }

    database = QSqlDatabase();
    QSqlDatabase::removeDatabase(getDatabaseName());

    return QSqlError();
}
