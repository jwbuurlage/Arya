#include "Commands.h"
#include "Config.h"
#include "Console.h"
#include "Sounds.h"
#include "common/Logger.h"
#include "Files.h"
namespace Arya
{
    template<> CommandHandler* Singleton<CommandHandler>::singleton = 0;

    CommandHandler::CommandHandler()
    {
    };

    CommandHandler::~CommandHandler()
    {
    }

    bool CommandHandler::init()
    {
        addCommandListener("consoleColor", this);
        addCommandListener("hide", this);
        addCommandListener("set", this);
        addCommandListener("get", this);
        addCommandListener("getVarValue", this);
        addCommandListener("setVarValue", this);
        addCommandListener("PLAYSOUND", this);
        addCommandListener("PLAYMUSIC", this);
        addCommandListener("STOPSOUND", this);
        addCommandListener("STOPMUSIC", this);

        return true;
    }

    void CommandHandler::addCommandListener(string command, CommandListener* listener)
    {
        commandListeners.insert(make_pair(command, listener));
    }

    void CommandHandler::removeCommandListener(string command, CommandListener* listener)
    {
        pair<commandListenerIterator, commandListenerIterator> range = commandListeners.equal_range(command);
        for(commandListenerIterator iter = range.first; iter != range.second; ++iter)
        {
            if( iter->second == listener )
            {
                commandListeners.erase(iter);
            }
        }
    }

    bool CommandHandler::handleCommand(string command)
    {
        bool flag = true;
        if(splitLineCommand(command) == "consoleColor")
        {
            float float1 = 0.0f;
            float float2 = 0.0f;
            float float3 = 0.0f;
            std::stringstream parser;
            parser << splitLineParameters(command);
            parser >> float1 >> float2 >> float3;
            Console::shared().changeConsoleColor(float1, float2, float3);
            Config::shared().editConfigFile(command);
        }
        if(splitLineCommand(command) == "hide")
        {
            if(splitLineParameters(command) == "console") Console::shared().toggleVisibilityConsole();
        }
        if(splitLineCommand(command) == "get")
        {
            string variableName;
            string type;
            std::stringstream parser;
            parser << splitLineParameters(command);
            parser >> variableName >> type;
            if(type == "int" || type == "Int" || type == "integer" || type == "Integer" || type == "INT")
            {
                int output = Config::shared().getCvarInt(variableName);
                LOG_INFO("The value of " << variableName << " is: " << output);
            }
            else if(type == "float" || type == "FLOAT" || type == "Float")
            {
                float output = Config::shared().getCvarFloat(variableName);
                LOG_INFO("The value of " << variableName << " is: " << output);
            }
            else if(type == "bool" || type == "BOOL" || type == "Bool")
            {
                bool output = Config::shared().getCvarBool(variableName);
                LOG_INFO("The value of " << variableName << " is: " << output);
            }
            else if(type == "string" || type == "STRING" || type == "String")
            {
                string output = Config::shared().getCvarString(variableName);
                LOG_INFO("The value of " << variableName << " is: " << output);
            }
            else
            {
                LOG_WARNING("Type of " << variableName << " not recognised!");
            }
        }
        if(splitLineCommand(command) == "set")
        {
            string variableName;
            string value;
            string type;
            std::stringstream parser;
            parser << splitLineParameters(command);
            parser >> variableName >> value >> type;
            if(type == "int" || type == "Int" || type == "integer" || type == "Integer" || type == "INT")
            {
                Config::shared().setCvar(variableName, value, TYPE_INTEGER);
            }
            else if(type == "float" || type == "FLOAT" || type == "Float")
            {
                Config::shared().setCvar(variableName, value, TYPE_FLOAT);
            }
            else if(type == "bool" || type == "BOOL" || type == "Bool")
            {
                Config::shared().setCvar(variableName, value, TYPE_BOOL);
            }
            else if(type == "string" || type == "STRING" || type == "String")
            {
                Config::shared().setCvar(variableName, value, TYPE_STRING);
            }
            else LOG_WARNING("Type of " << variableName << " not recognised!");
        }
        int count = 0;
        if(splitLineCommand(command) == "PLAYSOUND")
        {
            count = SoundManager::shared().play("testSound.wav");
            SoundManager::shared().setLoopSound("testSound.wav", count, 0.01, true);
            if(count == -1000) LOG_WARNING("SoundFile testSound.wav not found!");
        }
        if(splitLineCommand(command) == "PLAYMUSIC")
        {
            if(SoundManager::shared().play("testMusic.wav") == -1000) LOG_WARNING("MusicFile testMusic.wav not found!");
        }
        if(splitLineCommand(command) == "STOPMUSIC")
        {
            SoundManager::shared().stopMusic("testMusic.wav");
        }
        if(splitLineCommand(command) == "STOPSOUND")
        {
            SoundManager::shared().setLoopSound("testSound.wav", count, 0.5, false);
            SoundManager::shared().stopSound("testSound.wav", count, 0.001);
        }
        else flag = false;
        return flag;
    }

    void CommandHandler::onCommand(string command) // alleen op eerste woord zoeken
    {
        pair<commandListenerIterator, commandListenerIterator> range = commandListeners.equal_range(splitLineCommand(command));
        if(range.first == range.second)
        {
            LOG_WARNING("Command " << command << " received but no command registered");
        }
        else
        {
            for(commandListenerIterator iter = range.first; iter != range.second; ++iter)
            {
                iter->second->handleCommand(command);
            }
        }
    }

    string CommandHandler::splitLineCommand(string command)
    {
        int spaceFinder = 0;
        spaceFinder = command.find(" ", 0);
        if((unsigned)spaceFinder == std::string::npos)
        {
            return command;
        }
        int spaceCount = command.find_first_of(" ", 0);
        return command.substr(0, spaceCount);
    }

    string CommandHandler::splitLineParameters(string command)
    {
        int spaceFinder = 0;
        spaceFinder = command.find(" ", 0);
        if((unsigned)spaceFinder == std::string::npos)
        {
            return "";
        }
        int spaceCount = command.find_first_of(" ", 0);
        return command.substr(spaceCount + 1, command.length() - spaceCount - 1);
    }
}
