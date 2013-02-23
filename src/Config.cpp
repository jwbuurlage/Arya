#include <sstream>
#include "common/Logger.h"
#include "Config.h"
#include "Console.h"
using std::stringstream;
using std::fstream;
namespace Arya
{
    template<> Config* Singleton<Config>::singleton = 0;

    Config::Config()
    {
        configFile = 0;
    };

    Config::~Config()
    {
        cleanup();
    }
    bool Config::init()
    {
        if(loadConfigFile("config.txt")){return true;}
        else return false;
    }

    void Config::cleanup()
    {
        if(configFile != 0) FileSystem::shared().releaseFile(configFile);
        configFile = 0;
    }

    bool Config::loadConfigFile(string configFileName)
    {
        configFile = FileSystem::shared().getFile(configFileName);
        if(configFile == 0) return false;
        std::stringstream fileStream(configFile->getData());
        string regel;
        while(true)
        {
            getline(fileStream,regel);
            if(!fileStream.good()) break;
            if(regel.empty()) continue;
            if(regel.substr(0,3) == "var") continue;
            if(regel[0] == '#') continue;
            Console::shared().onCommand(regel);
        }
        return true;
    }

    void Config::editConfigFile(string edit)
    {
        bool flag = false;
        std::ofstream outputFile((FileSystem::shared().getApplicationPath() + "config.txt").c_str());
        if(!outputFile.is_open()) 
        {
            LOG_WARNING("Could not open output config file!");
            return;
        }
        string editCommand = edit.substr(0, edit.find_first_of(" "));
        std::stringstream fileStream(configFile->getData());
        string regel;
        while(true)
        {
            getline(fileStream,regel);
            if(!fileStream.good()) break;
            if(regel.empty())
            {
                outputFile << regel << std::endl;
                continue;
            }
            else
            {
                string regelCommand = regel.substr(0, edit.find_first_of(" "));
                if(regelCommand == editCommand)
                {
                    outputFile << edit << std::endl;
                    flag = true;
                }
                else
                {
                    outputFile << regel << std::endl;
                }
            }
        }
        if(flag == false) outputFile << edit << std::endl;
        outputFile.close();
    }
    string Config::getVarValue(string variableName)
    {
        if(!init())
        {
            LOG_WARNING("Error loading Config File!");
            return "";
        }
        else
        {
            std::stringstream fileStream(configFile->getData());
            string regel;
            while(true)
            {
                getline(fileStream,regel);
                if(!fileStream.good()) break;
                if(regel.substr(0,3) == "var" && regel.substr(4,variableName.size()) == variableName)
                {
                    return regel.substr(5 + variableName.size(), regel.size() - 5 - variableName.size());
                }
                else continue;
            }
        }
    }
    void Config::setVarValue(string variableName, string value)
    {
        bool flag = false;
        std::ofstream outputFile((FileSystem::shared().getApplicationPath() + "config.txt").c_str());
        if(!outputFile.is_open()) 
        {
            LOG_WARNING("Could not open output config file!");
            return;
        }
        std::stringstream fileStream(configFile->getData());
        string regel;
        while(true)
        {
            getline(fileStream,regel);
            if(!fileStream.good()) break;
            if(regel.empty())
            {
                outputFile << regel << std::endl;
                continue;
            }
            else
            {
                string regelVarAndValue = regel.substr(4, regel.size()-4);
                string regelVar = regel.substr(4, regelVarAndValue.find_first_of(" "));
                if(regelVar == variableName)
                {
                    outputFile << "var " << variableName << " " << value << std::endl;
                    flag = true;
                }
                else
                {
                    outputFile << regel << std::endl;
                }
            }
        }
        if(flag == false) outputFile << "var " << variableName << " " << value << std::endl;
        outputFile.close();

    }
}
