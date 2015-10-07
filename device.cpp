#include "device.h"

#include <stdlib.h>

#include <sstream>
#include <iostream>

#include "database.h"

/**
 * @brief Device::Device
 * @param p_port
 * @param p_baudRate
 */
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

/**
 * @brief Device::~Device
 */
Device::~Device()
{
    m_serialPort->close();
    delete m_serialPort;

    delete m_dataByteArray;
}

/**
 * @brief Device::getPPM
 * @return
 */
int
Device::getPPM()
{
    return m_ppm;
}

/**
 * @brief Device::getTemperatures
 * @return
 */
std::vector<float>
Device::getTemperatures()
{
    return m_temperatures;
}

/**
 * @brief Device::getId
 * @return
 */
std::string
Device::getId()
{
    return m_id;
}

/**
 * @brief Device::getRecordedData Get all data recorded by this device from a range of dates.
 * @param p_initialDate
 * @param p_finalDate
 * @return The recorded data as a string. Ex: deviceID date:144:32:32:32 date:144:32:32:32...
 */
std::string
Device::getRecordedData(int p_initialDate, int p_finalDate)
{
    DeviceRecord deviceRecord = getRegisteredDataByDevice(m_id, p_initialDate, p_finalDate);
    std::stringstream dataString;
    dataString << deviceRecord.deviceID;
    for (RegisteredData registeredData : deviceRecord.registeredData) {
        dataString << " " << registeredData.date << ":" << registeredData.ppm;
        for (float temperature : registeredData.temperatures) {
            dataString << ":" << temperature;
        }
    }
//    std::cout << dataString.str();
    return dataString.str();
}

/**
 * @brief Device::getData
 */
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

/**
 * @brief Device::recordAllData
 */
void
Device::recordAllData()
{
    recordData(QString(m_id.c_str()), m_ppm, m_temperatures);
}
