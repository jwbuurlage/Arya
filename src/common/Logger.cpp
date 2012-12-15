#include "common/Logger.h"
#include <sstream>
#include <iostream>

namespace Arya
{
	template<> Logger* Singleton<Logger>::singleton = 0;

	Logger::Logger(void)
	{
		consoleLogLevel = INFO | WARNING | ERROR | CRITICALERROR | DEBUG;
		fileLogLevel = WARNING | ERROR | CRITICALERROR;
		currentLogLevel = NONE;
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
        streambuff.str(std::string());
        streambuff.seekp(0);
        streambuff.clear();
	}
}
