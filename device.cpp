#include "device.h"

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
QVector<float>
Device::getTemperatures()
{
    return m_temperatures;
}

/**
 * @brief Device::getId
 * @return
 */
QString
Device::getId()
{
    return m_id;
}

/**
 * @brief Device::getData Get sensor data from arduino device
 */
void
Device::getData()
{
    //    (*m_dataByteArray).append(m_serialPort->readAll());
    int size = m_serialPort->bytesAvailable();
    if (size >= m_maxArraySize * 3) {
//        m_dataByteArray->append(m_serialPort->read(m_maxArraySize * 3));
        m_dataByteArray->append(m_serialPort->readAll());
        //
        QString _test(*m_dataByteArray);

        QStringList _test_list = _test.split('\n');
        QString _stripped_data = _test_list.at(2);

        QStringList sensorData = _stripped_data.split(' ');

        m_temperatures.clear();
        for (QStringList::const_iterator it = sensorData.begin(); it != sensorData.end(); ++it)
            if (it == sensorData.begin())
                m_ppm = ((QString)*it).toInt();
            else
                m_temperatures.append(((QString)*it).toFloat());

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
    recordData(m_id, m_ppm, m_temperatures);
}
