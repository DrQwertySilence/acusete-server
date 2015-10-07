#include "application.h"

#include <stdlib.h>
#include <time.h>

#include <iostream>
#include <sstream>
#include <string>

#include <QDebug>

/**
 * @brief Application::Application
 * @param argc
 * @param argv
 */
Application::Application(int &argc, char **argv) :
    QCoreApplication(argc, argv),
    m_data(new Data(this)),
    m_serialTimer(new QTimer(this)),
    m_serialTimerDelay(250),
    /// Server
    m_webSocketServer(new QWebSocketServer(QStringLiteral("acusete server"), QWebSocketServer::NonSecureMode, this))
{
    m_serialTimer->start(m_serialTimerDelay);

    /// Websocket server port
    std::vector<std::string> args = m_data->getConfiguration()->readFile("../share/acusete/server/port");
    int port = atoi(args[0].c_str());

    if (m_webSocketServer->listen(QHostAddress::Any, port)) {
        QObject::connect(m_webSocketServer, &QWebSocketServer::newConnection,
                         this, &Application::onNewConnection);
        QObject::connect(m_webSocketServer, &QWebSocketServer::closed,
                         this, &Application::closed);
    }

    ///Signal and slot stuff
    for (Device *device : m_data->getDevices()) {
        QObject::connect(m_serialTimer, &QTimer::timeout,
                         device, &Device::getData);
        QObject::connect(device, &Device::dataReceived,
                         this, &Application:: workOnSerialData);
    }

    QObject::connect(this, &Application::timerReceived,
                     this, &Application::setTimer);
}

/**
 * @brief Application::~Application
 */
Application::~Application()
{
    delete m_data;
    delete m_serialTimer;

    ///Server
    m_webSocketServer->close();
    delete m_webSocketServer;
    qDeleteAll(m_webSocketClients.begin(), m_webSocketClients.end());
}

/// TODO: Should be implemented in Device Class
/**
 * @brief Application::workOnSerialData
 */
void
Application::workOnSerialData()
{
    processSerialData(m_data->getDevices(), 400, 35);
}

/**
 * @brief Application::processSerialData Check if there is anormal data. Anormal data is the one that comes from variables PPM and Temperature captures by a device that bypass the maxPPM (more than) and minTemperature (less than) values.
 * @param p_devices
 * @param p_maxPPM
 * @param p_minTemperature
 */
void
Application::processSerialData(std::vector<Device*> p_devices, const int p_maxPPM, const float p_minTemperature)
{
    for (Device *device : p_devices) {
        if (device->getPPM() > p_maxPPM) {
            for (float temperature : device->getTemperatures())
                if (temperature < p_minTemperature)
                    startSensorAlarm();
        } else
            stopSensorAlarm();
    }
}

/**
 * @brief Application::getSerialDataString
 * @param p_devices
 * @return
 */
std::string
Application::getSerialDataString(std::vector<Device*> p_devices)
{
    std::stringstream string;
    for (Device *device : p_devices) {
        string << "Device: " << device->getId() << " Data: " << device->getPPM();
        for (float temperature : device->getTemperatures()) {
            string << " " << temperature;
        }
        string << '\n';
    }
    return string.str();
}

/**
 * @brief Application::startAlarm
 * @param p_alarm The alarm that is going to be started.
 */
void
Application::startAlarm(QSound *p_alarm)
{
    if(p_alarm->isFinished())
        p_alarm->play();
}

/**
 * @brief Application::stopAlarm
 * @param p_alarm The alarm that is going to be stoped
 */
void
Application::stopAlarm(QSound * p_alarm)
{
    if(!p_alarm->isFinished())
        p_alarm->stop();
}

/**
 * @brief Application::startTimerAlarm Start a timer (soft) alarm
 */
void
Application::startTimerAlarm()
{
    startAlarm(m_data->getTimerAlarm());
}

/**
 * @brief Application::stopTimerAlarm Stop a timer (soft) alarm
 */
void
Application::stopTimerAlarm()
{
    stopAlarm(m_data->getTimerAlarm());
}

/**
 * @brief Application::startSensorAlarm Start a sensor (hard) alarm
 */
void
Application::startSensorAlarm()
{
    startAlarm(m_data->getSensorAlarm());
}

/**
 * @brief Application::stopSensorAlarm Stop a sensor (hard) alarm
 */
void
Application::stopSensorAlarm()
{
    stopAlarm(m_data->getSensorAlarm());
}

/**
 * @brief Application::setTimer creates an alarm in a data object
 * @param pMilliseconds
 */
void
Application::setTimer(int pMilliseconds)
{
    m_data->addTimer(pMilliseconds);
}

/**
 * @brief Application::getData Getter of data object
 * @return the data object
 */
Data*
Application::getData()
{
    return m_data;
}

/**
 * @brief Application::printTest TEST FUNCTION: Delete if possible
 */
void
Application::printTest()
{
    std::cout << m_data->getTimers().size();
}

///Websocket Server

/**
 * @brief Application::onNewConnection Set the events when client interact with the server.
 */
void
Application::onNewConnection()
{
    QWebSocket *pSocket = m_webSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &Application::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &Application::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &Application::socketDisconnected);

    m_webSocketClients << pSocket;
}



/// Refactor this shit!!!
/**
 * @brief Application::processTextMessage Do something when a client send a message to the server.
 * @param message The message received.
 */
void
Application::processTextMessage(QString p_message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        std::stringstream stream;
        stream.str(p_message.toStdString());
        std::string word;
        std::vector<std::string> words;

        for (int i = 0; stream.good(); ++i) {
            std::getline(stream, word, ' ');
            words.push_back(word);
        }

        if (words.front() == "/alert") {
            if (words.size() == 1) {
                pClient->sendTextMessage("/alert up");
                startTimerAlarm();
            } else if (words.at(1) == "start") {
                pClient->sendTextMessage("/alert up");
                startTimerAlarm();
            } else if (words.at(1) == "stop") {
                pClient->sendTextMessage("/alert down");
                stopTimerAlarm();
            }

        } else if (words.front() == "/setTimer") {
            pClient->sendTextMessage("timer setted");
            if (words.size() == 1)
                timerReceived(0);
            else {
                int milliseconds = atoi(words.at(1).c_str());
                if (milliseconds < 0)
                    milliseconds = 0;
                timerReceived(milliseconds);
            }

        } else if (words.front() == "/sendMessage" || words.front() == "/sm") {
            if (words.size() == 1)
                pClient->sendTextMessage("No message setted");
            else {
                pClient->sendTextMessage(words.at(1).c_str());
            }

        } else if (words.front() == "/getTimers") {
            std::string timers = "/displayTimers ";
            timers.append(m_data->getFormatedTimers());
            pClient->sendTextMessage(timers.c_str());

        } else if (words.front() == "/getSensorData") {
            std::stringstream sensorData; // Ex: /displaySensorData device1:1442107814:144:32 device2:1442107814:144:32...

            sensorData << "/displaySensorData";

            for (Device *device : m_data->getDevices()) {
                sensorData << " " << device->getId();
                sensorData << ":" << time(NULL);
                sensorData << ":" << std::to_string(device->getPPM());
                for (auto temperature : device->getTemperatures()) {
                    sensorData << ":" << std::to_string(temperature);
                }
            }

            pClient->sendTextMessage(sensorData.str().c_str());
        } else if (words.front() == "/getRecordedData") {
            // STUB
            std::string message = std::string();
            message.append("/recordedDataForGraph ");
            for (Device *device : m_data->getDevices())
                if (device->getId() == words.at(1)) {
                    message.append(device->getRecordedData(atoi(words.at(2).c_str()), atoi(words.at(3).c_str())));
                    pClient->sendTextMessage(QString(message.c_str()));
                    break;
                }
        }
    }
}

/**
 * @brief Application::processBinaryMessage Not used: should be more efficient than working with string
 * @param message Binary menssage received
 */
void
Application::processBinaryMessage(QByteArray p_message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        pClient->sendBinaryMessage(p_message);
    }
}

/**
 * @brief Application::socketDisconnected
 */
void
Application::socketDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        m_webSocketClients.removeAll(client);
        client->deleteLater();
    }
}
