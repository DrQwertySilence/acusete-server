#include "database.h"

/**
 * @brief checkTableExistence
 * @param p_database
 * @param p_table
 * @return
 */
bool checkTableExistence(QSqlDatabase p_database, QString p_table)
{
    QStringList tables = p_database.tables();
    if (tables.contains(p_table))
        return true;
    return false;
}

/**
 * @brief checkTableExistence
 * @param p_database
 * @param p_tables
 * @return
 */
bool checkTableExistence(QSqlDatabase p_database, QStringList p_tables)
{
    QStringList tables = p_database.tables();
    for (QString table : p_tables)
        if (!tables.contains(table))
            return false;
    return true;
}

/**
 * @brief getSQLDriver Convenient method to get the SQL Driver string.
 * @return The name of the SQL driver
 */
const QString getSQLDriver()
{
    return "QSQLITE";
}

/**
 * @brief getDatabaseLocation Get the database location file path.
 * @return The physical location of the SQL database.
 */
const QString getDatabaseLocation()
{
    return "../share/acusete/datalog.sqlite";
}

/**
 * @brief getDatabaseName Get the database name string.
 * @return The name of the database.
 */
const QString getDatabaseName()
{
    return "record";
}

/**
 * @brief getRegisteredDataByDevice Get data from the application database.
 * @param p_deviceID Device id.
 * @param p_initialDate First date to look for record.
 * @param p_finalDate Last date to look for record.
 * @return Returns a QJson with all the data of the records found.
 */
QJsonObject
getRegisteredDataByDevice(QString p_deviceID, int p_initialDate, int p_finalDate)
{
    QSqlDatabase database = QSqlDatabase::addDatabase(getSQLDriver(), getDatabaseName());
    database.setDatabaseName(getDatabaseLocation());

    if (!database.open()) {
        return QJsonObject();
    }

    QJsonObject deviceRecord;
    deviceRecord.insert("ID", p_deviceID);

    QSqlQuery query = QSqlQuery(database);
    query.prepare("select * from datarecord where device = ? and date >= ? and date <= ? order by date");
//    query.prepare("select * from datarecord where device = ? and date >= ? and date <= ? and rowid % 10 = 0 order by date");
    query.addBindValue(p_deviceID);
    query.addBindValue(p_initialDate);
    query.addBindValue(p_finalDate);

    QSqlQuery temperatureQuery = QSqlQuery(database);
    temperatureQuery.prepare("select * from temperaturerecord where datarecordId = ?");

    QJsonArray jsonRecords;
    if (query.exec()) {
        while (query.next()) {
            QJsonObject jsonRecord;
            jsonRecord.insert("date", query.value(2).toInt());
            jsonRecord.insert("ppm", query.value(3).toInt());

            QJsonArray temperatures;
            temperatureQuery.addBindValue(query.value(0).toInt());
            if (temperatureQuery.exec()) {
                while (temperatureQuery.next()) {
                    temperatures.append(temperatureQuery.value(2).toDouble());
                }
            }
            jsonRecord.insert("temperatures", temperatures);

            jsonRecords.append(jsonRecord);
        }
    }

    deviceRecord.insert("records", jsonRecords);

    database = QSqlDatabase();
    QSqlDatabase::removeDatabase(getDatabaseName());

    return deviceRecord;
}

/**
 * @brief recordDeviceData Helper funtion. Bind values to a query and executes it.
 * @param p_query The query.
 * @param p_deviceId The id of Device.
 * @param p_time The time when the data was captured by Device.
 * @param p_ppm The ppm captured by Device.
 * @return Returns the id of the record registered by this functon.
 */
QVariant recordDeviceData(QSqlQuery &p_query, QString &p_deviceId, qint64 p_time, qint64 &p_ppm)
{
    p_query.addBindValue(p_deviceId);
    p_query.addBindValue(p_time);
    p_query.addBindValue(p_ppm);
    p_query.exec();
    return p_query.lastInsertId();
}

/**
 * @brief recordTemperature Helper function. Bind values to a query and executes t.
 * @param p_query The query.
 * @param p_registerId Id given by the database  when recordDeviceData is executed.
 * @param p_temperature The temperature that is going to be registered.
 */
void recordTemperature(QSqlQuery &p_query, QVariant &p_registerId, float &p_temperature)
{
    p_query.addBindValue(p_registerId);
    p_query.addBindValue(p_temperature);
    p_query.exec();
}

/**
 * @brief recordData Store given data (from a Device) to the application database.
 * @param p_deviceId Device id.
 * @param p_ppm Ppm registered by Device.
 * @param p_temperatures Temperatures registered by Device.
 * @return
 */
QSqlError recordData(QString p_deviceId, qint64 p_ppm, QVector<float> p_temperatures)
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
