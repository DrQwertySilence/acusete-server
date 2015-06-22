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

    QByteArray *m_dataByteArray; // Where data from serial is temporaly stored
    int m_maxArraySize; // Max size of the buffer

    /// Server
    QWebSocketServer *m_webSocketServer;
    QList<QWebSocket*> m_webSocketClients;

    /// Serial Data
    int m_ppm;
    std::vector<float> m_temperature;

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
public slots:
    void getDataSerial();
    void startSensorAlert();
    void stopSensorAlert();
    void startTimerAlert();
    void stopTimerAlert();
    void setTimer(int pMilliseconds);

public:
    Application(int &argc, char **argv);
    virtual ~Application();

    Data* getData();
};

#endif // APPLICATION_H
