#ifndef APPLICATION_H
#define APPLICATION_H

#include <QCoreApplication>
#include <QTimer>
#include <QList>
#include <QByteArray>
#include <QWebSocket>
#include <QWebSocketServer>

#include "data.h"
#include "configuration.h"

class Application : public QCoreApplication
{
    Q_OBJECT
private:
    Data *m_data;
    QTimer *m_serialTimer; // Interval of time that the program waits to process received data
    int m_serialTimerDelay;
    int m_ppmMax;
    double m_temperatureMin;
    /// Websocket Server
    QWebSocketServer *m_webSocketServer;
    QList<QWebSocket*> m_webSocketClients;
    /// Serial Data
    void processSerialData(QVector<Device*> p_devices, int p_maxPPM, float p_minTemperature);

signals:
    ///Server
    void closed();
    void timerReceived(int pMilliseconds);
private slots:
    ///Server
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();

    void startAlarm(QSound *p_alarm);
    void stopAlarm(QSound *p_alarm);

public slots:
    void workOnSerialData();
    void startSensorAlarm();
    void stopSensorAlarm();
    void startTimerAlarm();
    void stopTimerAlarm();
    void setTimer(int pMilliseconds);

public:
    Application(int &argc, char **argv);
    virtual ~Application();
};

#endif // APPLICATION_H
