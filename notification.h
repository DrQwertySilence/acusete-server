#ifndef INOTIFICATION_H
#define INOTIFICATION_H

#include <QWebSocket>

class Notification
{
public:
    Notification();
    int notify(int pValue);
};

#endif // INOTIFICATION_H
