//Singleton

//USAGE:
//Logger::shared() << Logger::WARNING << "Texture not found. File: " << someVariable << ". Try again!" << endLog;
//Do NOT forget the endLog at the end of a log message.
//OR use
//LOG_WARNING("Texture not found. File: " << someVariable << ". Try again!");

#pragma once
#include "Singleton.h"
#include <sstream>

//sometimes the word DEBUG is defined
#ifdef DEBUG
#undef DEBUG
#endif

//Note: the ; at the end is left out so that you can use this as a function with a ; at the end:  LOG_WARNING("Hello");
#define LOG_WARNING(MSG)		Arya::Logger::shared() << Arya::Logger::WARNING		<< MSG << Arya::endLog
#define LOG_ERROR(MSG)			Arya::Logger::shared() << Arya::Logger::ERROR		<< MSG << Arya::endLog
#define LOG_CRITICALERROR(MSG)	Arya::Logger::shared() << Arya::Logger::CRITICALERROR	<< MSG << Arya::endLog
#define LOG_INFO(MSG)			Arya::Logger::shared() << Arya::Logger::INFO		<< MSG << Arya::endLog
#define LOG_DEBUG(MSG)			Arya::Logger::shared() << Arya::Logger::DEBUG		<< MSG << Arya::endLog

namespace Arya
{
	class Logger : public Singleton<Logger>
	{
	public:
		Logger();
		~Logger();

		//Log level:
		enum LOGLEVEL{ //Powers of 2
			NONE=0,
			DEBUG = 1,
			INFO = 2,
			WARNING = 4,
			ERROR = 8,
			CRITICALERROR = 16
		};

		//combine the flags like DEBUG | ERROR | CRITICALERROR
		int consoleLogLevel;
		int fileLogLevel;

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
		switch(lvl){
		case Logger::CRITICALERROR:
			logger.streambuff.str("Critical ERROR: ");
			break;
		case Logger::ERROR:
			logger.streambuff.str("ERROR: ");
			break;
		case Logger::WARNING:
			logger.streambuff.str("Warning: ");
			break;
		case Logger::INFO:
			logger.streambuff.str("Info: ");
			break;
		case Logger::DEBUG:
			logger.streambuff.str("Debug: ");
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
