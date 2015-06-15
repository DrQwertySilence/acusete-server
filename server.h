#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QtCore/QList>
#include <QtCore/QByteArray>
#include <QWebSocket>
#include <QWebSocketServer>

class Server : public QObject
{
    Q_OBJECT
private:
    QWebSocketServer *mServer;
    QList<QWebSocket*> mClients;
signals:
    void closed();
    void timerReceived(int pMilliseconds);
private slots:
    void onNewConnection();
    void processTextMessage(QString message);
    void processBinaryMessage(QByteArray message);
    void socketDisconnected();
public:
    Server(int pPort, QObject *pParent = nullptr);
    virtual ~Server();
};

#endif // SERVER_H
