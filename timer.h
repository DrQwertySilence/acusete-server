#ifndef TIMER_H
#define TIMER_H

#include <QTimer>
#include <QObject>

#include "device.h"

class Timer : public QObject
{
    Q_OBJECT
private:
    static int idCount;
    static int getNextId();

    int m_id;
    int m_initialTime;
    int m_remainingTime;
    QTimer* m_timer;

public:
    Timer(int p_time, QObject *p_parent = nullptr);
    virtual ~Timer();
    //
    void resume();
    void pause();
    void stop();
    void restart();

    int getId();
    int getInitialTime();
    int getRemainingTime();

private slots:
    void ontimeout();
signals:
    void removed(Timer *p_timer);
    //
    void started();
    void paused();
    void stopped();
    void restarted();
};

#endif // TIMER_H
