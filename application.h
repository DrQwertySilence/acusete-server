#ifndef APPLICATION_H
#define APPLICATION_H

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

    QByteArray *mDataByteArray;
    int mMaxArraySize;

signals:
    void isAlerted();
private slots:
    void printTest();
public slots:
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
