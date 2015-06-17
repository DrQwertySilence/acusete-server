#include "data.h"

#include <fstream>
#include <sstream>
#include <string>

#include <chrono>
#include <ctime>

#include <application.h>

// TODO: Implement here the object creation.
Data::Data(QObject *pParent):
    QObject(pParent),
    mAlarm(new QSound("../share/acusete/sound/alert-short.wav")),
    mServer(new Server(1234, this))
{
    mAlarm->setLoops(-1);
    mDevices.push_back(new Device("COM6", QSerialPort::BaudRate::Baud9600));
}

Data::~Data()
{
    delete mAlarm;
}

void
readFile(std::ifstream pFile)
{
    pFile.open("../share/acusete/devices/arduino");
    std::stringstream line;
    std::string tempLine;
    std::string word;
    if(pFile.is_open()) {
        while(pFile.good()) {
            std::getline(pFile, tempLine);
            line.str(tempLine);
            while(line.good()) {
                std::getline(line, word, ' ');
            }
        }
        pFile.close();
    } else {
        // TODO: implement log system qq
    }
}

std::vector<Device*>
Data::getDevices()
{
    return mDevices;
}

QSound*
Data::getAlarm()
{
    return mAlarm;
}

void
Data::addTimer(int pMilliseconds)
{
    Timer *tempTimer = new Timer(pMilliseconds, this);
    mTimers.push_back(tempTimer);
}

Server*
Data::getServer()
{
    return mServer;
}

std::vector<Timer *>
Data::getTimers()
{
    return mTimers;
}

std::string
Data::getFormatedTimers(/*std::vector<Timer*> mTimers*/)
{
    std::string formatedTimers;

    for (auto timer :this-> mTimers) {
        if (timer != (*mTimers.begin()))
            formatedTimers.append(" ");
        timer->remainingTime();

        int timerRT = timer->remainingTime();

        int hours = timerRT / (60 * 60 * 1000);
        int minutes = (timerRT - (hours * 60 * 60 * 1000)) / (60 * 1000);
        int seconds = (timerRT - (minutes * 60 * 1000) -  (hours * 60 * 60 * 1000)) / 1000;
        int milliseconds = timerRT - (seconds * 1000) - (minutes * 1000 * 60) - (hours * 60 * 60 * 1000);

        formatedTimers.append(std::to_string(hours) +
                              ":" +
                              std::to_string(minutes) +
                              ":" +
                              std::to_string(seconds) +
                              ":" +
                              std::to_string(milliseconds));
    }

    return formatedTimers;
}
