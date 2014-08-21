//
// Usage:
// LogError << "Message. Info: " << myvariable << " more info" << endl;
//

#pragma once

#include <functional>
#include <string>
#include <sstream>
#include <fstream>

#define LogError    (Arya::logger << Arya::L_ERROR)
#define LogWarning  (Arya::logger << Arya::L_WARNING)
#define LogInfo     (Arya::logger << Arya::L_INFO)
#define LogDebug    (Arya::logger << Arya::L_DEBUG)

namespace Arya
{

    enum LOGLEVEL{ //Must be powers of 2 to allow combining
        L_NONE = 0,
        L_DEBUG = 1,
        L_INFO = 2,
        L_WARNING = 4,
        L_ERROR = 8,
        L_CRITICALERROR = 16
    };

    class Logger
    {
        public:
            Logger();
            ~Logger();

            bool setOutputFile(const char* filename);
            void closeOutputFile();

            //! combine the LOGLEVEL flags to specify
            //! which log messages show up at that place
            int stdoutLogLevel;
            int fileLogLevel;
            int callbackLogLevel;

            void setLoggerCallback(std::function<void(const std::string&)> func){ callbackFunc = func; return; }

            //TODO: If this does not compile, revert to old method
            //This might not compile on MSVC++
            template<class T>
            inline Logger& operator<<(const T& t) {
                streambuff << t;
                return *this;
            }

        private:
            std::function<void(const std::string&)> callbackFunc;
            std::ofstream filestream;

            //TODO: THIS SHOULD BE THREAD-LOCAL
            std::stringstream streambuff;
            LOGLEVEL currentLogLevel; //log level of the message in the streambuff

            void flush();

            friend Logger& endLog(Logger& logger);
            friend Logger& operator<<(Logger& logger, LOGLEVEL lvl);
    };

    //So you can do logger << endLog;
    Logger& endLog(Logger& logger);

    inline Logger& operator<<(Logger& logger, Logger& (*manipulatorFunc)(Logger&) ){
        return manipulatorFunc(logger);
    };

    inline Logger& operator<<(Logger& logger, LOGLEVEL lvl){
        logger.currentLogLevel = lvl;
        //Reset the stringstream
        logger.streambuff.str(std::string());
        logger.streambuff.seekp(0);
        logger.streambuff.clear();
        switch(lvl){
            case L_CRITICALERROR:
                logger.streambuff << "Critical ERROR: ";
                break;
            case L_ERROR:
                logger.streambuff << "ERROR: ";
                break;
            case L_WARNING:
                logger.streambuff << "Warning: ";
                break;
            case L_INFO:
                logger.streambuff << "Info: ";
                break;
            case L_DEBUG:
                logger.streambuff << "Debug: ";
                break;
            default:
                break;
        };
        return logger;
    }


    //All << operations that stringstream can do:
    //template<class T>
    //    inline Logger& operator<<(Logger& logger, const T& t){
    //        logger.getStream() << t;
    //        return logger;
    //    }

    extern Logger logger;

}

