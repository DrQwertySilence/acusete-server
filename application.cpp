#include "application.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>

#include <QProcess>

Application::Application(int &argc, char **argv):
    QCoreApplication(argc, argv),
    mData(new Data(this)),
    mTimer(new QTimer(this)),
    mAlertTimer(new QTimer(this)),
    mDelay(100), mFrameCount(0),
    mMaxFrameCount(3),
    mDataByteArray(new QByteArray()),
    mMaxArraySize(24)
{
    mTimer->start(mDelay);
    mAlertTimer->setSingleShot(true);
    mAlertTimer->start(5000);
    QObject::connect(mTimer, &QTimer::timeout,
                     this, &Application::getDataSerial);
    QObject::connect(mData->getServer(), &Server::timerReceived,
                     this, &Application::setTimer);
}

Application::~Application()
{
    delete mTimer;
}

void
Application::getDataSerial()
{
    (*mDataByteArray) += mData->getDevices()[0]->getSerialPort()->readAll();

    if(mDataByteArray->size() > mMaxArraySize)
    {
        std::stringstream stream;
        stream.str(QString(*mDataByteArray).toStdString());
        std::string stripeddata = std::string("");
        int i = 0;

        while(stream.good()){
            if(i == 1){
                std::getline(stream, stripeddata);
                break;
            } else {
                std::getline(stream, stripeddata);
                ++i;
            }
        }
        std::cout << stripeddata << '\n';

        mDataByteArray->clear();
    }
}

void
Application::isAlerted()
{

}

void
Application::startAlert()
{
    if(mData->getAlarm()->isFinished()) {
        mData->getAlarm()->play();
        std::cout << "ALERT, ALERT, ALERT, DANGER, DANGER!!!" << std::endl;
    }
}

void
Application::stopAlert()
{
    if(!mData->getAlarm()->isFinished()) {
        mData->getAlarm()->stop();
        std::cout << "OK, ALL DONE!!!" << std::endl;
    }
}

void
Application::setTimer(int pMilliseconds)
{
    mData->addTimer(pMilliseconds);
}
