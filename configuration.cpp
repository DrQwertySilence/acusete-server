#include "configuration.h"

#include <sstream>
#include <string>
#include <fstream>

/**
 * @brief Configuration::Configuration
 */
Configuration::Configuration()
{

}

/**
 * @brief Configuration::getSensorAlarmPath
 * @return
 */
std::string
Configuration::getSensorAlarmPath()
{
    return "../share/acusete/sound/alert-short.wav";
}

/**
 * @brief Configuration::getTimerAlarmPath
 * @return
 */
std::string
Configuration::getTimerAlarmPath()
{
    return "../share/acusete/sound/alarm2.wav";
}

/**
 * @brief Configuration::getDeviceListPath
 * @return
 */
std::string
Configuration::getDeviceListPath()
{
    return "../etc/acusete/arduino";
}

/**
 * @brief Configuration::readFile
 * @param pPath
 * @return
 */
std::vector<std::string>
Configuration::readFile(std::string pPath)
{
    std::vector<std::string> args;
    std::ifstream file;
    file.open(pPath);
    std::stringstream line;
    std::string tempLine;
    std::string word;
    if(file.is_open()) {
        while(file.good()) {
            std::getline(file, tempLine);
            line.str(tempLine);
            while(line.good()) {
                std::getline(line, word, ' ');
                args.push_back(word);
            }
        }
        file.close();
    } else {
    }
    return args;
}
