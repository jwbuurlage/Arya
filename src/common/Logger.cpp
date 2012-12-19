#include "common/Logger.h"
#include "Console.h"
#include <sstream>
#include <iostream>

namespace Arya
{
	template<> Logger* Singleton<Logger>::singleton = 0;

	Logger::Logger(void)
	{
		consoleLogLevel = L_INFO | L_WARNING | L_ERROR | L_CRITICALERROR | L_DEBUG;
		fileLogLevel = L_WARNING | L_ERROR | L_CRITICALERROR;
        gameConsoleLogLevel = L_INFO | L_WARNING | L_ERROR | L_CRITICALERROR | L_DEBUG;
		currentLogLevel = L_NONE;
        filestream.open("logarya.txt");
	}

	Logger::~Logger(void)
	{
        filestream.close();
	}

	void Logger::log(LOGLEVEL type, const char* logText)
	{
		*this << type << logText << endLog;
	}

	void Logger::flush()
	{
		if( consoleLogLevel & currentLogLevel ){
			std::cout << streambuff.str() << std::endl;
		}
		if( fileLogLevel & currentLogLevel ){
			//Note: When doing file output, also prepend timestamp
            filestream << streambuff.str() << std::endl;
		}
        if(gameConsoleLogLevel & currentLogLevel)
        {
            if(&Console::shared()) Console::shared().addOutputText(streambuff.str());
        }
        streambuff.str(std::string());
        streambuff.seekp(0);
        streambuff.clear();
	}
}
