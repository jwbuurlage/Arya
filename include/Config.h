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

        private:
            bool loadConfigFile(string configFileName);
            File* configFile;
    };
}
