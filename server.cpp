#include "server.h"

#include <sstream>
#include <string>
#include <vector>

#include <stdlib.h>

#include "application.h"

Server::Server(int pPort, QObject *pParent) :
    QObject(pParent),
    mWebSocketServer(new QWebSocketServer(QStringLiteral("acusete server"), QWebSocketServer::NonSecureMode, this)),
    mWebSocketClients()
{
    if (mWebSocketServer->listen(QHostAddress::Any, pPort)) {
        connect(mWebSocketServer, &QWebSocketServer::newConnection,
                    this, &Server::onNewConnection);
        connect(mWebSocketServer, &QWebSocketServer::closed,
                    this, &Server::closed);
    }
}

Server::~Server()
{
    mWebSocketServer->close();
    qDeleteAll(mWebSocketClients.begin(), mWebSocketClients.end());
}

void
Server::onNewConnection()
{
    QWebSocket *pSocket = mWebSocketServer->nextPendingConnection();

    connect(pSocket, &QWebSocket::textMessageReceived, this, &Server::processTextMessage);
    connect(pSocket, &QWebSocket::binaryMessageReceived, this, &Server::processBinaryMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &Server::socketDisconnected);

    mWebSocketClients << pSocket;
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
            std::string timers = "/displayTimers ";
            timers.append(((Data*)this->parent())->getFormatedTimers());
            pClient->sendTextMessage(timers.c_str()); //"/displayTimers 01:10 02:20 03:30"
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
        mWebSocketClients.removeAll(client);
        client->deleteLater();
    }
}
