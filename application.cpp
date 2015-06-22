#include "application.h"

#include <iostream>
#include <sstream>
#include <string>

#include <stdlib.h>

Application::Application(int &argc, char **argv) :
    QCoreApplication(argc, argv),
    m_data(new Data(this)),
    m_serialTimer(new QTimer(this)),
    m_serialTimerDelay(100),
    m_dataByteArray(new QByteArray()),
    m_maxArraySize(24),
    /// Server
    m_webSocketServer(new QWebSocketServer(QStringLiteral("acusete server"), QWebSocketServer::NonSecureMode, this)),
    m_webSocketClients()
{
    m_serialTimer->start(m_serialTimerDelay);

    /// Websocket server port
    std::vector<std::string> args = m_data->getConfiguration()->readFile("../share/acusete/server/port");
    int port = atoi(args[0].c_str());

    if (m_webSocketServer->listen(QHostAddress::Any, port)) {
        connect(m_webSocketServer, &QWebSocketServer::newConnection,
                this, &Application::onNewConnection);
        connect(m_webSocketServer, &QWebSocketServer::closed,
                this, &Application::closed);
    }

    ///Signal and slot stuff
    QObject::connect(m_serialTimer, &QTimer::timeout,
                     this, &Application:: getDataSerial);
    QObject::connect(this, &Application::timerReceived,
                     this, &Application::setTimer);
}

Application::~Application()
{
    delete m_serialTimer;

    ///Server
    m_webSocketServer->close();
    qDeleteAll(m_webSocketClients.begin(), m_webSocketClients.end());
}

/// TODO: Should be implemented to support more that one device.
/// Advice: pass a device as a parameter :)
void
Application::getDataSerial()
{
    (*m_dataByteArray) += m_data->getDevices()[0]->getSerialPort()->readAll();

    if (m_dataByteArray->size() > m_maxArraySize) {
        std::string stripedData;
        std::stringstream stream;
        stream.str(QString(*m_dataByteArray).toStdString());

        int i = 0;
        while (stream.good() && i <= 1) {
            if (i == 1) {
                std::getline(stream, stripedData);
                break;
            } else {
                std::getline(stream, stripedData);
                ++i;
            }
        }
        ///
        std::vector<std::string> sensorData;
        std::stringstream line;
        std::string word;

        line.str(stripedData);

        while(line.good()) {
            std::getline(line, word, ' ');
            sensorData.push_back(word);
        }
        ///
        m_temperature.clear();
        for (auto it = sensorData.begin(); it != sensorData.end(); ++it) {
            if (it == sensorData.begin())
                m_ppm = atoi((*it).c_str());
            else
                m_temperature.push_back(atof((*it).c_str()));
        }
        ///
        std::cout << "Data: " << m_ppm << " " << m_temperature.at(0) << '\n';
        ///Should be configurable
        if (m_ppm > 400 && m_temperature.at(0) < 35)
            startSensorAlert();
        else
            stopSensorAlert();
        ///
        m_dataByteArray->clear();
    }
}

void
Application::startTimerAlert()
{
    if(m_data->getTimerAlarm()->isFinished()) {
        m_data->getTimerAlarm()->play();
        std::cout << "ALERT, ALERT, ALERT, DANGER, DANGER!!!" << std::endl;
    }
}

void
Application::stopTimerAlert()
{
    if (!m_data->getTimerAlarm()->isFinished()) {
        m_data->getTimerAlarm()->stop();
        std::cout << "OK, ALL DONE!!!" << std::endl;
    }
}

void
Application::startSensorAlert()
{
    if(m_data->getSensorAlarm()->isFinished()) {
        m_data->getSensorAlarm()->play();
    }
}

void
Application::stopSensorAlert()
{
    if (!m_data->getSensorAlarm()->isFinished()) {
        m_data->getSensorAlarm()->stop();
    }
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
                startTimerAlert();
            } else if (words.at(1) == "start") {
                pClient->sendTextMessage("/alert up");
                startTimerAlert();
            } else if (words.at(1) == "stop") {
                pClient->sendTextMessage("/alert down");
                stopTimerAlert();
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
            pClient->sendTextMessage(timers.c_str()); //"/displayTimers 01:10 02:20 03:30"
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
