#include "common/Singleton.h"
#include "Files.h"
#include <string>
using std::string;
namespace Arya
{
    class Config : public Singleton<Config>
    {
        public:
            Config();
            ~Config();

            bool init();
            void cleanup();
            void editConfigFile(string edit);
            string getVarValue(string variableName);
            void setVarValue(string variableName, string value);
        private:
            bool loadConfigFile(string configFileName);
            File* configFile;
    };
}
