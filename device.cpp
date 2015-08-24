#include "device.h"

#include <stdlib.h>

#include <sstream>
#include <iostream>

#include "database.h"

Device::Device(QString p_port, QSerialPort::BaudRate p_baudRate):
    m_serialPort(new QSerialPort()),
    m_id("Derp"),
    m_dataByteArray(new QByteArray()),
    m_maxArraySize(10),
    m_ppm(0)
{
    m_serialPort->setBaudRate(p_baudRate);
    m_serialPort->setPortName(p_port);

    m_serialPort->setDataBits(QSerialPort::DataBits::Data7);
    m_serialPort->setParity(QSerialPort::Parity::EvenParity);
    m_serialPort->setStopBits(QSerialPort::StopBits::OneStop);
    m_serialPort->setFlowControl(QSerialPort::FlowControl::NoFlowControl);

    m_serialPort->open(QIODevice::ReadOnly);
}

Device::~Device()
{
    m_serialPort->close();
    delete m_serialPort;

    delete m_dataByteArray;
}

int
Device::getPPM()
{
    return m_ppm;
}

std::vector<float>
Device::getTemperatures()
{
    return m_temperatures;
}

std::string
Device::getId()
{
    return m_id;
}

void
Device::getData()
{
    //    (*m_dataByteArray).append(m_serialPort->readAll());
    int size = m_serialPort->bytesAvailable();
    if (size >= m_maxArraySize * 3) {
        m_dataByteArray->append(m_serialPort->read(m_maxArraySize * 3));
        ///
        std::string stripedData;
        std::stringstream stream;
        stream.str(QString(*m_dataByteArray).toStdString());

        int i = 0;
        while (stream.good() && i <= 1) {
            if (i == 2) {
                std::getline(stream, stripedData);
                break;
            } else {
                std::getline(stream, stripedData);
                ++i;
            }
        }
        ///
        std::vector<std::string> sensorData;
        std::stringstream line;
        std::string word;

        line.str(stripedData);

        while(line.good()) {
            std::getline(line, word, ' ');
            sensorData.push_back(word);
        }
        ///
        m_temperatures.clear();
        for (auto it = sensorData.begin(); it != sensorData.end(); ++it) {
            if (it == sensorData.begin())
                m_ppm = atoi((*it).c_str());
            else
                m_temperatures.push_back(atof((*it).c_str()));
        }
        ///
        m_serialPort->clear();
        m_dataByteArray->clear();
        dataReceived();

        recordAllData();
    }
}

// SQL stuff
void
Device::recordAllData()
{
    recordData(QString(m_id.c_str()), m_ppm, m_temperatures);
}
