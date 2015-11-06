#ifndef TIMER_H
#define TIMER_H

#include <QTimer>
#include <QObject>

#include "device.h"

enum TIMER_STATE{
    UNKNOW = -1,
    COUNTING = 0,
    PAUSED = 1,
    STOPPED = 2,
    DESTROYED = 3
};

class Timer : public QObject
{
    Q_OBJECT
private:
    static int idCount;
    static int getNextId();
    int m_id;
    QString m_description;
    int m_initialTime;
    int m_remainingTime;
    QTimer* m_timer;
    int m_state;

public:
    Timer(int p_time, QString p_description, QObject *p_parent = nullptr);
    virtual ~Timer();
    //
    void resume();
    void pause();
    void stop();
    void restart();
    void destroy();

    int getId();
    QString getDescription();
    int getRemainingTime();

private slots:
    void ontimeout();
signals:
    void removed(Timer *p_timer);
    //
    void resumed();
    void paused();
    void stopped();
    void restarted();
    void destroyed();
};

#endif // TIMER_H
