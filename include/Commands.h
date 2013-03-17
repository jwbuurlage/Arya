#pragma once
#include "common/Singleton.h"
#include "common/Listeners.h"
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

            void addCommandListener(string command, CommandListener* listener);
            void removeCommandListener(string command, CommandListener* listener);
            //remove a listener for all its registered commands:
            void removeCommandListener(CommandListener* listener);

            void onCommand(string command);

			string splitLineCommand(string command);
			string splitLineParameters(string command);
        private:

            multimap<string, CommandListener*> commandListeners;
            typedef multimap<string,CommandListener*>::iterator commandListenerIterator;
    };
}
