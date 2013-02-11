#include <sstream>
#include "common/Logger.h"
#include "Config.h"
#include "Console.h"
using std::stringstream;
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
            if(regel[0] == '#') continue;
            Console::shared().onCommand(regel);
        }
        return true;
    }
}
