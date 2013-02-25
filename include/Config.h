#include "common/Singleton.h"
#include "Files.h"
#include <string>
#include <sstream>
#include <map>
using std::string;
using std::map;

namespace Arya
{
    enum ValueType
    {
        TYPE_UNKNOWN = 0,
        TYPE_STRING,
        TYPE_INTEGER,
        TYPE_FLOAT,
        TYPE_BOOL
    };
    struct cvar
    {
        ValueType type;
        string value;
        cvar(ValueType t, string v) : type(t), value(v) {};
        ~cvar(){};
        int getInt(){int ret; std::istringstream(value) >> ret; return ret;}
        float getFloat(){float ret; std::istringstream(value) >> ret; return ret;}
    };
    class Config : public Singleton<Config>
    {
        public:
            Config();
            ~Config();
            bool init();
            void cleanup();
            void editConfigFile(string edit);
            cvar* getCvar(string name);
            int getCvarInt(string name);
            float getCvarFloat(string name);
            void setCvarWithoutSave(string name, string value, ValueType type = TYPE_STRING);
            void setCvar(string name, string value, ValueType type = TYPE_STRING);
            void setCvarInt(string name, int value);
            void setCvarFloat(string name, float value);
        private:
            typedef map<string,cvar> cvarContainer;
            cvarContainer cvarList;
            bool loadConfigFile(string configFileName);
            bool loadConfigFileAfterRootInit(string configFileName);
            void updateConfigFile();
            File* configFile;
    };
}
