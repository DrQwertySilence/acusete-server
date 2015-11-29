#include "device.h"

#include "database.h"

/**
 * @brief Device::Device Constructor.
 * @param p_id The identifier that a user give to this device.
 * @param p_port The serial port that the application looks to connect to this device.
 * @param p_baudRate The frequency at what this application should read device data.
 * @param p_ppmMax Warning parameter.
 * @param p_temperatureMin Warning parameter.
 */
Device::Device(QString p_id, QString p_port, QSerialPort::BaudRate p_baudRate, int p_ppmMax, double p_temperatureMin):
    m_serialPort(new QSerialPort()),
    m_id(p_id),
    m_dataByteArray(new QByteArray()),
    m_maxArraySize(10),
    m_ppm(0),
    m_ppmMax(p_ppmMax),
    m_temperatureMin(p_temperatureMin)
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
 * @brief Device::~Device Closes the serial port and deletes it.
 */
Device::~Device()
{
    m_serialPort->close();
    delete m_serialPort;

    delete m_dataByteArray;
}

/**
 * @brief Device::checkData Check if the data that this device have captured is under the warning parameters
 */
void
Device::checkData()
{
    if (m_ppm < m_ppmMax)
        dataChecked(0);
    else {
        for (float temperature : m_temperatures)
            if (temperature < m_temperatureMin) {
                dataChecked(1);

                recordAllData();

                return;
            }
    }
}

/**
 * @brief Device::getPPM Getter.
 * @return Returns the ppm value captured by this device.
 */
int
Device::getPPM()
{
    return m_ppm;
}

/**
 * @brief Device::getTemperatures Getter.
 * @return Returns the list of temperatures captured by this device.
 */
QVector<float>
Device::getTemperatures()
{
    return m_temperatures;
}

/**
 * @brief Device::getId Getter.
 * @return Returns the id of this device.
 */
QString
Device::getId()
{
    return m_id;
}

/**
 * @brief Device::getData Gets sensor data from arduino device
 */
void
Device::getData()
{
//    (*m_dataByteArray).append(m_serialPort->readAll());
    int size = m_serialPort->bytesAvailable();
    if (size >= m_maxArraySize * 3) {
//        m_dataByteArray->append(m_serialPort->read(m_maxArraySize * 3));
        m_dataByteArray->append(m_serialPort->readAll());

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

//        recordAllData();
    }
}

/**
 * @brief Device::recordAllData Stores the data registered by this device in the application database.
 */
void
Device::recordAllData()
{
    recordData(m_id, m_ppm, m_temperatures);
}
