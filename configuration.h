#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <vector>
#include <string>

class Configuration
{
public:
    std::string getSensorAlarmPath();
    std::string getTimerAlarmPath();
    std::string getDeviceListPath();

    std::vector<std::string> readFile(std::string pPath);

    Configuration();
};

#endif // CONFIGURATION_H
