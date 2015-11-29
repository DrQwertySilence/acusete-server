#include "application.h"

/**
 * @brief Application::Application Constructor
 * @param argc Number of arguments given at application start.
 * @param argv Value of each argument given at application start.
 */
Application::Application(int &argc, char **argv) :
    QCoreApplication(argc, argv),
    m_serialTimer(new QTimer(this)),
    m_data(new Data(m_serialTimer, this)),
    m_serialTimerDelay(1000),
    m_ppmMax(Configuration::configuration.getWarningConfiguration().value("ppm").toInt()),
    m_temperatureMin(Configuration::configuration.getWarningConfiguration().value("temperature").toDouble()),
    /// Server
    m_webSocketServer(new QWebSocketServer("acusete server", QWebSocketServer::NonSecureMode, this))
{
    m_serialTimer->start(m_serialTimerDelay);

    /// Websocket server port
    QJsonObject serverConfig = Configuration::configuration.getServerConfiguration();
    int port = serverConfig.value("port").toInt(0);

    if (m_webSocketServer->listen(QHostAddress::Any, port)) {
        QObject::connect(m_webSocketServer, SIGNAL(newConnection()),
                         this, SLOT(onNewConnection()));
        QObject::connect(m_webSocketServer, SIGNAL(closed()),
                         this, SLOT(socketDisconnected()));
    }

    QObject::connect(this, SIGNAL(timerReceived(int,QString)),
                     m_data, SLOT(addTimer(int,QString)));
}

/**
 * @brief Application::~Application Closes websocket server and then delete all websocket clients.
 */
Application::~Application()
{
    m_webSocketServer->close();
    qDeleteAll(m_webSocketClients.begin(), m_webSocketClients.end());
    delete m_webSocketServer;
    delete m_data;
    delete m_serialTimer;
}

/**
 * @brief Application::startAlarm Start given alarm.
 * @param p_alarm The alarm that is going to be started.
 */
void
Application::startAlarm(QSoundEffect *p_alarm)
{
    if(!p_alarm->isPlaying())
        p_alarm->play();
}

/**
 * @brief Application::stopAlarm Stop given alarm.
 * @param p_alarm The alarm that is going to be stopped.
 */
void
Application::stopAlarm(QSoundEffect *p_alarm)
{
    if(p_alarm->isPlaying())
        p_alarm->stop();
}

/**
 * @brief Application::startTimerAlarm Start the (soft) alarm.
 */
void
Application::startTimerAlarm()
{
    startAlarm(m_data->getTimerAlarm());
}

/**
 * @brief Application::stopTimerAlarm Stop the (soft) alarm.
 */
void
Application::stopTimerAlarm()
{
    stopAlarm(m_data->getTimerAlarm());
}

/**
 * @brief Application::startSensorAlarm Based on a state start of stop the (strong) alarm.
 * @param p_state The state that determine if the alarm should be started or stopped.
 */
void
Application::startSensorAlarm(int p_state)
{
    if (p_state)
        startAlarm(m_data->getSensorAlarm());
    else
        stopAlarm(m_data->getSensorAlarm());
}

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

/**
 * @brief sendMessage Send a message to a client via websocket.
 * @param p_message The type of message. Depending of the content of this variable the client do something with the data.
 * @param p_data Message content as a QJsonObject.
 */
void
Application::sendMessage(QWebSocket *p_client, QString p_message, QJsonObject p_data)
{
    QJsonDocument document;
    QJsonObject obj {
        {"message", p_message},
        {"msgData", p_data}
    };
    document.setObject(obj);
    p_client->sendTextMessage(document.toJson(QJsonDocument::Compact));
}

/**
 * @brief sendMessage Send a message to a client via websocket.
 * @param p_message The type of message. Depending of the content of this variable the client do something with the data.
 * @param p_data Message content as a QJsonArray
 */
void
Application::sendMessage(QWebSocket *p_client, QString p_message, QJsonArray p_data)
{
    QJsonDocument document;
    QJsonObject obj {
        {"message", p_message},
        {"msgData", p_data}
    };
    document.setObject(obj);
    p_client->sendTextMessage(document.toJson(QJsonDocument::Compact));
}

/**
 * @brief Application::processTextMessage Do something when a client send a message to the server.
 * @param message The message received.
 */
void
Application::processTextMessage(QString p_message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        QStringList words = p_message.split(' ');
        QString command = words.front();

        if (command == "/alert") {
            int value = 1;
            if (words.size() == 1 || words.at(1) == "start")
                value = 1;
            else if (words.at(1) == "stop")
                value = 0;

            QJsonObject subObj {{"value", value}};

            sendMessage(pClient, "alert", subObj);

            stopTimerAlarm();

        } else if (command == "/setTimer") {
            if (words.size() == 1)
                timerReceived(0, "");
            else {
                int milliseconds = words.at(1).toInt();
                if (milliseconds < 0)
                    milliseconds = 0;
                timerReceived(milliseconds, words.at(2));
            }

        } else if (command == "/getTimers") {
            sendMessage(pClient, "displayTimers",
                        m_data->getFormatedTimers());

        } else if (command == "/getSensorData") {
            sendMessage(pClient, "displaySensorData",
                        m_data->getFormatedDeviceData());

        } else if (command == "/getRecordedData") {
            sendMessage(pClient, "recordedDataForGraph",
                        m_data->getRecordedData(words.at(2).toInt(), words.at(3).toInt()));

        } else if (command == "/resumeTimer") {
            m_data->getTimerById(words.at(1).toInt())->resume();
            QJsonObject subObj {{"id", words.at(1).toInt()}};
            sendMessage(pClient, "timerResumed",subObj);

        } else if (command == "/pauseTimer") {
            m_data->getTimerById(words.at(1).toInt())->pause();
            QJsonObject subObj {{"id", words.at(1).toInt()}};
            sendMessage(pClient, "timerPaused", subObj);

        } else if (command == "/stopTimer") {
            m_data->getTimerById(words.at(1).toInt())->stop();
            QJsonObject subObj {{"id", words.at(1).toInt()}};
            sendMessage(pClient, "timerStopped", subObj);

        } else if (command == "/restartTimer") {
            m_data->getTimerById(words.at(1).toInt())->restart();
            QJsonObject subObj {{"id", words.at(1).toInt()}};
            sendMessage(pClient, "timerRestarted", subObj);

        } else if (command == "/destroyTimer") {
            m_data->getTimerById(words.at(1).toInt())->destroy();
            QJsonObject subObj {{"id", words.at(1).toInt()}};
            sendMessage(pClient, "timerDestroyed", subObj);
        }
    }
}

/**
 * @brief Application::processBinaryMessage Not used: should be more efficient than working with strings
 * @param message Binary message received
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
 * @brief Application::socketDisconnected Kill all client connections to the server.
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
