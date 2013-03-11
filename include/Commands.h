#pragma once
#include "common/Listeners.h"
#include "common/Singleton.h"
#include <vector>
#include <string>
#include <map>
using std::vector;
using std::string;
using std::multimap;
using std::pair;

namespace Arya
{
    class CommandHandler : public Singleton<CommandHandler>, public InputListener, public CommandListener
    {
        public:
            CommandHandler();
            virtual ~CommandHandler();

            bool isInitialized() const { return initialized; }
            bool init();

            void addCommandListener(string command, CommandListener* listener);
            void removeCommandListener(string command, CommandListener* listener);
            bool handleCommand(string command);

            void onCommand(string command);

        private:
            bool initialized;

            string splitLineCommand(string command);
            string splitLineParameters(string command);

            multimap<string, CommandListener*> commandListeners;
            typedef multimap<string,CommandListener*>::iterator commandListenerIterator;
    };
}
