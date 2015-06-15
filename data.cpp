#include "data.h"

#include <fstream>
#include <sstream>
#include <string>

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
