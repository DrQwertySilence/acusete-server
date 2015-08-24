#ifndef APPLICATION_H
#define APPLICATION_H

#include <QCoreApplication>
#include <QTimer>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QWebSocket>
#include <QWebSocketServer>

#include "data.h"

class Application : public QCoreApplication
{
    Q_OBJECT
private:
    Data *m_data;

    QTimer *m_serialTimer; // Interval of time that the program waits to process received data
    int m_serialTimerDelay;

    /// Websocket Server
    QWebSocketServer *m_webSocketServer;
    QList<QWebSocket*> m_webSocketClients;

    /// Serial Data
    int m_ppm;
    std::vector<float> m_temperatures;

    void processSerialData(std::vector<Device*> p_devices, const int p_maxPPM, const float p_minTemperature);
    std::string getSerialDataString(std::vector<Device*> p_devices);

signals:
    void isAlerted();

    ///Server
    void closed();
    void timerReceived(int pMilliseconds);
private slots:
    void printTest();

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

    Data* getData();
};

#endif // APPLICATION_H
