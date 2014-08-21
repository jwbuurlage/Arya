//
// Usage:
// LogError << "Message. Info: " << myvariable << " more info" << endl;
//

#pragma once

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
        L_ERROR = 8
    };

    class Logger
    {
        public:
            Logger();
            ~Logger();

            //! combine the LOGLEVEL flags to specify
            //! which log messages show up at that place
            int stdoutLogLevel;
            int fileLogLevel;
            int callbackLogLevel;

        private:
            //std::ofsteam filesteam;

            //TODO: THIS SHOULD BE THREAD-LOCAL
            //std::stringstream streambuff;
            //LOGLEVEL currentLogLevel; //log level of the message in the streambuff
    };
}

extern Arya::Logger logger;

