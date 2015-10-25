#include "application.h"

#include <QDebug>

#include <QJsonDocument>
#include <QJsonObject>

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
    m_webSocketServer(new QWebSocketServer("acusete server", QWebSocketServer::NonSecureMode, this))
{
    m_serialTimer->start(m_serialTimerDelay);

    /// Websocket server port
    QJsonObject serverConfig = Configuration::configuration.getServerConfiguration();
    int port = serverConfig.value("port").toInt(0);

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
Application::processSerialData(QVector<Device*> p_devices, int p_maxPPM, float p_minTemperature)
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
QString
Application::getSerialDataString(QVector<Device*> p_devices)
{
    QString line;
    for (Device *device : p_devices) {
        line.append("Device: " + device->getId() + " Data: " + QString::number(device->getPPM()));
        for (float temperature : device->getTemperatures()) {
            line.append(" " + QString::number(temperature));
        }
        line.append('\n');
    }
    return line;
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
        QStringList words = p_message.split(' ');
        QString command = words.front();

        if (command == "/alert") {
            int value = 1;
            if (words.size() == 1 || words.at(1) == "start")
                value = 1;
            else if (words.at(1) == "stop")
                value = 0;

            QJsonDocument document;
            QJsonObject subObj {{"value", value}};
            QJsonObject obj {
                {"message", "alert"},
                {"msgData", subObj}
            };
            document.setObject(obj);
            pClient->sendTextMessage(document.toJson(QJsonDocument::Compact));
            stopTimerAlarm();

        } else if (command == "/setTimer") {
            if (words.size() == 1)
                timerReceived(0);
            else {
                int milliseconds = words.at(1).toInt();
                if (milliseconds < 0)
                    milliseconds = 0;
                timerReceived(milliseconds);
            }

        } else if (command == "/getTimers") {
            QJsonArray timerArray = m_data->getFormatedTimers();

            QJsonDocument document;
            QJsonObject obj {
                {"message", "displayTimers"},
                {"msgData", timerArray}
            };
            document.setObject(obj);

            pClient->sendTextMessage(document.toJson(QJsonDocument::Compact));

        } else if (command == "/getSensorData") {
            QJsonArray devicesData = m_data->getFormatedDeviceData();

            QJsonDocument document;
            QJsonObject obj {
                {"message", "displaySensorData"},
                {"msgData", devicesData}
            };
            document.setObject(obj);

            pClient->sendTextMessage(document.toJson(QJsonDocument::Compact));

        } else if (command == "/getRecordedData") {
            QJsonArray devicesRecords = m_data->getRecordedData(words.at(2).toInt(), words.at(3).toInt());

            QJsonDocument document;
            QJsonObject obj {
                {"message", "recordedDataForGraph"},
                {"msgData", devicesRecords}
            };
            document.setObject(obj);

            pClient->sendTextMessage(document.toJson(QJsonDocument::Compact));

        } else if (command == "/resumeTimer") {
            m_data->getTimerById(words.at(1).toInt())->resume();

            QJsonDocument document;
            QJsonObject subObj {{"id", words.at(1).toInt()}};
            QJsonObject obj {
                {"message", "timerResumed"},
                {"msgData", subObj}
            };
            document.setObject(obj);

            pClient->sendTextMessage(document.toJson(QJsonDocument::Compact));

        } else if (command == "/pauseTimer") {
            m_data->getTimerById(words.at(1).toInt())->pause();

            QJsonDocument document;
            QJsonObject subObj {{"id", words.at(1).toInt()}};
            QJsonObject obj {
                {"message", "timerPaused"},
                {"msgData", subObj}
            };
            document.setObject(obj);

            pClient->sendTextMessage(document.toJson(QJsonDocument::Compact));

        } else if (command == "/stopTimer") {
            m_data->getTimerById(words.at(1).toInt())->stop();

            QJsonDocument document;
            QJsonObject subObj {{"id", words.at(1).toInt()}};
            QJsonObject obj {
                {"message", "timerStopped"},
                {"msgData", subObj}
            };
            document.setObject(obj);

            pClient->sendTextMessage(document.toJson(QJsonDocument::Compact));

        } else if (command == "/restartTimer") {
            m_data->getTimerById(words.at(1).toInt())->restart();

            QJsonDocument document;
            QJsonObject subObj {{"id", words.at(1).toInt()}};
            QJsonObject obj {
                {"message", "timerRestarted"},
                {"msgData", subObj}
            };
            document.setObject(obj);

            pClient->sendTextMessage(document.toJson(QJsonDocument::Compact));

        } else if (command == "/destroyTimer") {
            m_data->getTimerById(words.at(1).toInt())->destroy();

            QJsonDocument document;
            QJsonObject subObj {{"id", words.at(1).toInt()}};
            QJsonObject obj {
                {"message", "timerDestroyed"},
                {"msgData", subObj}
            };
            document.setObject(obj);

            pClient->sendTextMessage(document.toJson(QJsonDocument::Compact));

        } else if (command == "/test_json") {
            QJsonDocument document;
            QJsonObject subObj {{"value", 1}};
            QJsonObject obj {
                {"message", "alert"},
                {"msgData", subObj}
            };
            document.setObject(obj);

            pClient->sendTextMessage(document.toJson(QJsonDocument::Compact));
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
