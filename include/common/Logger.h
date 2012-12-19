//Singleton

//USAGE:
//Logger::shared() << Logger::WARNING << "Texture not found. File: " << someVariable << ". Try again!" << endLog;
//Do NOT forget the endLog at the end of a log message.
//OR use
//LOG_WARNING("Texture not found. File: " << someVariable << ". Try again!");

#pragma once
#include "common/Singleton.h"
#include <sstream>
#include <fstream>

//Note: the ; at the end is left out so that you can use this as a function with a ; at the end:  LOG_WARNING("Hello");
#define LOG_WARNING(MSG)		Arya::Logger::shared() << Arya::Logger::L_WARNING		<< MSG << Arya::endLog
#define LOG_ERROR(MSG)			Arya::Logger::shared() << Arya::Logger::L_ERROR		<< MSG << Arya::endLog
#define LOG_CRITICALERROR(MSG)	Arya::Logger::shared() << Arya::Logger::L_CRITICALERROR	<< MSG << Arya::endLog
#define LOG_INFO(MSG)			Arya::Logger::shared() << Arya::Logger::L_INFO		<< MSG << Arya::endLog
#define LOG_DEBUG(MSG)			Arya::Logger::shared() << Arya::Logger::L_DEBUG		<< MSG << Arya::endLog

namespace Arya
{
	class Logger : public Singleton<Logger>
	{
	public:
		Logger();
		~Logger();

		//Log level:
		enum LOGLEVEL{ //Powers of 2
			L_NONE=0,
			L_DEBUG = 1,
			L_INFO = 2,
			L_WARNING = 4,
			L_ERROR = 8,
			L_CRITICALERROR = 16
		};

		//combine the flags like DEBUG | ERROR | CRITICALERROR
		int consoleLogLevel;
		int fileLogLevel;
        int gameConsoleLogLevel;

		//This can be used instead of the << operators
		//It will append a newline
		void log(LOGLEVEL type, const char* logText); //log will append newline

		//Ends the current log message
		//Flushes the text to console and file
		void flush();

		//Do NOT use this directly!
		//I wasnt able to make the templated << operator a friend of this class so
		//I had to make this function public
		std::stringstream& getStream(){ return streambuff; }
	private:
		std::stringstream streambuff;
        std::ofstream filestream;
		LOGLEVEL currentLogLevel; //The log type that is currently in the streambuff

		friend Logger& operator<<(Logger& logger, Logger::LOGLEVEL lvl);
	};

	//So you can do logger << endLog;
	inline Logger& endLog(Logger& logger){
		logger.flush();
		return logger;
	};
	inline Logger& operator<<(Logger& logger, Logger& (*manipulatorFunc)(Logger&) ){
		return manipulatorFunc(logger);
	};

	inline Logger& operator<<(Logger& logger, Logger::LOGLEVEL lvl){
		logger.currentLogLevel = lvl;
        //Reset the stringstream
        logger.streambuff.str(std::string());
        logger.streambuff.seekp(0);
        logger.streambuff.clear();
		switch(lvl){
		case Logger::L_CRITICALERROR:
			logger.streambuff << "Critical ERROR: ";
			break;
		case Logger::L_ERROR:
			logger.streambuff << "ERROR: ";
			break;
		case Logger::L_WARNING:
			logger.streambuff << "Warning: ";
			break;
		case Logger::L_INFO:
			logger.streambuff << "Info: ";
			break;
		case Logger::L_DEBUG:
			logger.streambuff << "Debug: ";
			break;
		default:
			break;
		};
		return logger;
	}

	//All << operations that stringstream can do:
	template<class T>
	inline Logger& operator<<(Logger& logger, const T& t){
		logger.getStream() << t;
		return logger;
	}

}
