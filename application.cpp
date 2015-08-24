#include "application.h"

#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <string>

#include <QDebug>

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
void
Application::workOnSerialData()
{
    processSerialData(m_data->getDevices(), 400, 35);
//    std::cout << getSerialDataString(m_data->getDevices());
}

/// Check if there is anormal data. Anormal data is the one that comes from variables PPM and Temperature captures by a device that bypass the maxPPM (more than) and minTemperature (less than) values.
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

void
Application::startAlarm(QSound *p_alarm)
{
    if(p_alarm->isFinished())
        p_alarm->play();
}

void
Application::stopAlarm(QSound * p_alarm)
{
    if(!p_alarm->isFinished())
        p_alarm->stop();
}

void
Application::startTimerAlarm()
{
    startAlarm(m_data->getTimerAlarm());
}

void
Application::stopTimerAlarm()
{
    stopAlarm(m_data->getTimerAlarm());
}

void
Application::startSensorAlarm()
{
    startAlarm(m_data->getSensorAlarm());
}

void
Application::stopSensorAlarm()
{
    stopAlarm(m_data->getSensorAlarm());
}

void
Application::setTimer(int pMilliseconds)
{
    m_data->addTimer(pMilliseconds);
}

Data*
Application::getData()
{
    return m_data;
}

void
Application::printTest()
{
    std::cout << m_data->getTimers().size();
}

///Websocket Server

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
void
Application::processTextMessage(QString message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {

        std::stringstream stream;
        stream.str(message.toStdString());
        std::vector<std::string> words;
        std::string word;

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
            // Should be multidevice
            std::string sensorData = "/displaySensorData ";
            sensorData.append(std::to_string(m_data->getDevices().at(0)->getPPM()));

            for (auto temperature : m_data->getDevices().at(0)->getTemperatures()) {
                sensorData.append(" " + std::to_string(temperature));
            }

            pClient->sendTextMessage(sensorData.c_str());
        }
    }
}

void
Application::processBinaryMessage(QByteArray message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        pClient->sendBinaryMessage(message);
    }
}

void
Application::socketDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        m_webSocketClients.removeAll(client);
        client->deleteLater();
    }
}
