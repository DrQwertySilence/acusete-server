#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <vector>
#include <string>

class Configuration
{
public:
    Configuration();
    std::vector<std::string> readFile(std::string pPath);
};

#endif // CONFIGURATION_H
