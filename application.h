#ifndef APPLICATION_H
#define APPLICATION_H

#include <vector>
#include <sstream>

#include <QCoreApplication>
#include <QTimer>

#include "data.h"

class Application : public QCoreApplication
{
private:
    Data *mData;

    QTimer *mTimer;
    QTimer *mAlertTimer;

    int mDelay;
    int mFrameCount;
    int mMaxFrameCount;
    QByteArray *mDataByteArray;
    int mMaxArraySize;

signals:
    void isAlerted();
public slots:
    void printTest();
    void getDataSerial();
    void startAlert();
    void stopAlert();
    void setTimer(int pMilliseconds);

public:
    Application(int &argc, char **argv);
    virtual ~Application();

    Data* getData();
};

#endif // APPLICATION_H
