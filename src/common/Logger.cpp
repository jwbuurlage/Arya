#include "common/Logger.h"
#include <sstream>
#include <iostream>

Arya::Logger AryaLogger;

namespace Arya
{
	Logger::Logger(void)
	{
		consoleLogLevel = L_INFO | L_WARNING | L_ERROR | L_CRITICALERROR | L_DEBUG;
		fileLogLevel = L_WARNING | L_ERROR | L_CRITICALERROR;
        callbackLogLevel = L_INFO | L_WARNING | L_ERROR | L_CRITICALERROR | L_DEBUG;
		currentLogLevel = L_NONE;
        callbackFunc = 0;
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
        if(callbackFunc)
            if(callbackLogLevel & currentLogLevel)
                callbackFunc(streambuff.str());

        streambuff.str(std::string());
        streambuff.seekp(0);
        streambuff.clear();
	}

    Logger& Logger::shared()
    {
        return AryaLogger;
    }
}
