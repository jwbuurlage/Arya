#pragma once

//This is basicly a copy of the Arya logger
//but with thread safe support

#include <sstream>
#include <fstream>

//Note: the ; at the end is left out so that you can use this as a function with a ; at the end:  LOG_WARNING("Hello");
#define GAME_LOG_WARNING(MSG)		GameLoggerInstance << GameLogger::L_WARNING	<< MSG << Arya::endLog
#define GAME_LOG_ERROR(MSG)			GameLoggerInstance << GameLogger::L_ERROR		<< MSG << Arya::endLog
#define GAME_LOG_CRITICALERROR(MSG)	GameLoggerInstance << GameLogger::L_CRITICALERROR	<< MSG << Arya::endLog
#define GAME_LOG_INFO(MSG)			GameLoggerInstance << GameLogger::L_INFO		<< MSG << Arya::endLog
#define GAME_LOG_DEBUG(MSG)			GameLoggerInstance << GameLogger::L_DEBUG		<< MSG << Arya::endLog

class GameLogger
{
public:
    GameLogger();
    ~GameLogger();

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
    std::stringstream& getStream();
private:
    //The following construction is to avoid Poco
    //includes in this header file.
    //Look up "Pimpl idiom" or "Chesire cat"
    class InternalStream *internalStream;

    std::ofstream filestream;
    LOGLEVEL currentLogLevel; //The log type that is currently in the streambuff

    friend GameLogger& operator<<(GameLogger& logger, GameLogger::LOGLEVEL lvl);
};

//So you can do logger << endLog;
inline GameLogger& endLog(GameLogger& logger){
    logger.flush();
    return logger;
};
inline GameLogger& operator<<(GameLogger& logger, GameLogger& (*manipulatorFunc)(GameLogger&) ){
    return manipulatorFunc(logger);
};

inline GameLogger& operator<<(GameLogger& logger, GameLogger::LOGLEVEL lvl){
    logger.currentLogLevel = lvl;
    //Reset the stringstream
    logger.getStream().str(std::string());
    logger.getStream().seekp(0);
    logger.getStream().clear();
    switch(lvl){
    case GameLogger::L_CRITICALERROR:
        logger.getStream() << "Critical ERROR: ";
        break;
    case GameLogger::L_ERROR:
        logger.getStream() << "ERROR: ";
        break;
    case GameLogger::L_WARNING:
        logger.getStream() << "Warning: ";
        break;
    case GameLogger::L_INFO:
        logger.getStream() << "Info: ";
        break;
    case GameLogger::L_DEBUG:
        logger.getStream() << "Debug: ";
        break;
    default:
        break;
    };
    return logger;
}

//All << operations that stringstream can do:
template<class T>
inline GameLogger& operator<<(GameLogger& logger, const T& t){
    logger.getStream() << t;
    return logger;
}

extern GameLogger GameLoggerInstance;
