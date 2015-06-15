#include "server.h"

#include <sstream>
#include <string>
#include <vector>

#include <stdlib.h>

#include <QTimer>

#include "application.h"

Server::Server(int pPort, QObject *pParent):
    QObject(pParent),
    mServer(new QWebSocketServer(QStringLiteral("acusete server"), QWebSocketServer::NonSecureMode, this)),
    mClients()
{
    if (mServer->listen(QHostAddress::Any, pPort)) {
        connect(mServer, &QWebSocketServer::newConnection,
                    this, &Server::onNewConnection);
        connect(mServer, &QWebSocketServer::closed,
                    this, &Server::closed);
        connect(this, &Server::timerReceived,
                    (Application*)this->parent()->parent(), &Application::setTimer);
    }
}

Server::~Server()
{
    mServer->close();
    qDeleteAll(mClients.begin(), mClients.end());
}

void
Server::onNewConnection()
{
    QWebSocket *pSocket = mServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &Server::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &Server::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &Server::socketDisconnected);

    mClients << pSocket;
}

void
Server::processTextMessage(QString message)
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
                ((Application*)this->parent()->parent())->startAlert();
            } else if (words.at(1) == "start") {
                pClient->sendTextMessage("/alert up");
                ((Application*)this->parent()->parent())->startAlert();
            } else if (words.at(1) == "stop") {
                pClient->sendTextMessage("/alert down");
                ((Application*)this->parent()->parent())->stopAlert();
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
            pClient->sendTextMessage("/displayTimers 01,10 02,20 03,30");
        }
    }
}

void
Server::processBinaryMessage(QByteArray message)
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient) {
        pClient->sendBinaryMessage(message);
    }
}

void
Server::socketDisconnected()
{
    QWebSocket *client = qobject_cast<QWebSocket *>(sender());
    if (client) {
        mClients.removeAll(client);
        client->deleteLater();
    }
}
