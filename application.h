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
    QTimer *m_serialTimer;
    Data *m_data;
    int m_serialTimerDelay;
    int m_ppmMax;
    double m_temperatureMin;

    QWebSocketServer *m_webSocketServer;
    QList<QWebSocket*> m_webSocketClients;

    void sendMessage(QWebSocket *p_client, QString p_message, QJsonObject p_data);
    void sendMessage(QWebSocket *p_client, QString p_message, QJsonArray p_data);

signals:
    void timerReceived(int p_milliseconds, QString p_description);

private slots:
    void onNewConnection();
    void processTextMessage(QString p_message);
    void processBinaryMessage(QByteArray p_message);
    void socketDisconnected();

    void startAlarm(QSoundEffect *p_alarm);
    void stopAlarm(QSoundEffect *p_alarm);

public slots:
    void startSensorAlarm(int p_state);
    void startTimerAlarm();
    void stopTimerAlarm();
    void setTimer(int p_milliseconds, QString p_description);

public:
    Application(int &argc, char **argv);
    virtual ~Application();
};

#endif // APPLICATION_H
