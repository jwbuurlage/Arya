#include "Logger.h"
#include <iostream>

namespace Arya
{
    //The global object
    Logger logger;

    Logger::Logger()
    {
		consoleLogLevel = L_INFO | L_WARNING | L_ERROR | L_CRITICALERROR | L_DEBUG;
		fileLogLevel = L_WARNING | L_ERROR | L_CRITICALERROR;
        callbackLogLevel = L_INFO | L_WARNING | L_ERROR | L_CRITICALERROR | L_DEBUG;
		currentLogLevel = L_NONE;
        callbackFunc = std::nullptr;
    }

    Logger::~Logger()
    {
        filestream.close();
    }

    bool Logger::setOutputFile(const char* filename)
    {
        filestream.open(filename);
        return filestream.is_open();
    }

    void Logger::closeOutputFile()
    {
        filestream.close();
    }

    Logger& endLog(Logger& logger)
    {
        logger.flush();
        return logger;
    }

    void Logger::flush()
    {
		if( consoleLogLevel & currentLogLevel ){
			std::cout << streambuff.str() << std::endl;
		}
		if( fileLogLevel & currentLogLevel ){
			//TODO: When doing file output, also prepend timestamp
            filestream << streambuff.str() << std::endl;
		}
        if(callbackFunc)
            if(callbackLogLevel & currentLogLevel)
                callbackFunc(streambuff.str());

        streambuff.str(std::string());
        streambuff.seekp(0);
        streambuff.clear();
	}

}
