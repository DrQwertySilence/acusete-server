#include "configuration.h"

#include <sstream>
#include <string>
#include <fstream>

Configuration::Configuration()
{

}

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
