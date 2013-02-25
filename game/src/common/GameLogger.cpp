#include "../../include/common/GameLogger.h"

#include "Arya.h"
#include "Poco/ThreadLocal.h"
#include <iostream>

GameLogger* GameLoggerInstance;

class InternalStream
{
    public:
        Poco::ThreadLocal<std::stringstream> streambuff;
};

GameLogger::GameLogger(void)
{
    if(!GameLoggerInstance) GameLoggerInstance = this;
    internalStream = new InternalStream;
    consoleLogLevel = L_INFO | L_WARNING | L_ERROR | L_CRITICALERROR | L_DEBUG;
    fileLogLevel = L_WARNING | L_ERROR | L_CRITICALERROR;
    gameConsoleLogLevel = L_INFO | L_WARNING | L_ERROR | L_CRITICALERROR | L_DEBUG;
    currentLogLevel = L_NONE;
    filestream.open("logaryagame.txt");
}

GameLogger::~GameLogger(void)
{
    filestream.close();
    delete internalStream;
}

std::stringstream& GameLogger::getStream()
{
    return *(internalStream->streambuff);
}

void GameLogger::log(LOGLEVEL type, const char* logText)
{
    *this << type << logText << endLog;
}

void GameLogger::flush()
{
    if( consoleLogLevel & currentLogLevel ){
        std::cout << getStream().str() << std::endl;
    }
    if( fileLogLevel & currentLogLevel ){
        //Note: When doing file output, also prepend timestamp
        filestream << getStream().str() << std::endl;
    }
#ifndef SERVERONLY
    if(gameConsoleLogLevel & currentLogLevel)
    {
        if(&Arya::Console::shared()) Arya::Console::shared().addOutputText(getStream().str());
    }
#endif
    getStream().str(std::string());
    getStream().seekp(0);
    getStream().clear();
}
