#ifndef DATABASE
#define DATABASE

#include <time.h>

#include <QtSql>
#include <QSqlError>
#include <QtCore>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

bool checkTableExistence(QSqlDatabase p_database, QString p_table);
bool checkTableExistence(QSqlDatabase p_database, QStringList p_tables);

const QString getSQLDriver();
const QString getDatabaseLocation();
const QString getDatabaseName();

QJsonObject getRegisteredDataByDevice(QString p_deviceID, int p_initialDate, int p_finalDate);
QVariant recordDeviceData(QSqlQuery &p_query, QString &p_deviceId, qint64 p_time, qint64 &p_ppm);
void recordTemperature(QSqlQuery &p_query, QVariant &p_registerId, float &p_temperature);
QSqlError recordData(QString p_deviceId, qint64 p_ppm, QVector<float> p_temperatures);

#endif // DATABASE
