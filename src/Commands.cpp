#include "Commands.h"
#include "Config.h"
#include "Console.h"
#include "Sounds.h"
#include "common/Logger.h"
#include "Files.h"
namespace Arya
{
	template<> CommandHandler* Singleton<CommandHandler>::singleton = 0;

	CommandHandler::CommandHandler()
	{
	};

	CommandHandler::~CommandHandler()
	{
	}

	void CommandHandler::addCommandListener(string command, CommandListener* listener)
	{
		commandListeners.insert(make_pair(command, listener));
	}

	void CommandHandler::removeCommandListener(string command, CommandListener* listener)
	{
		pair<commandListenerIterator, commandListenerIterator> range = commandListeners.equal_range(command);
		for(commandListenerIterator iter = range.first; iter != range.second; ++iter)
		{
			if( iter->second == listener )
			{
				commandListeners.erase(iter);
			}
		}
	}

	void CommandHandler::onCommand(string command) // alleen op eerste woord zoeken
	{
		pair<commandListenerIterator, commandListenerIterator> range = commandListeners.equal_range(splitLineCommand(command));
		if(range.first == range.second)
		{
			LOG_WARNING("Command " << command << " received but no command registered");
		}
		else
		{
			for(commandListenerIterator iter = range.first; iter != range.second; ++iter)
			{
				iter->second->handleCommand(command);
			}
		}
	}

	string CommandHandler::splitLineCommand(string command)
	{
		int spaceFinder = 0;
		spaceFinder = command.find(" ", 0);
		if((unsigned)spaceFinder == std::string::npos)
		{
			return command;
		}
		int spaceCount = command.find_first_of(" ", 0);
		return command.substr(0, spaceCount);
	}

	string CommandHandler::splitLineParameters(string command)
	{
		int spaceFinder = 0;
		spaceFinder = command.find(" ", 0);
		if((unsigned)spaceFinder == std::string::npos)
		{
			return "";
		}
		int spaceCount = command.find_first_of(" ", 0);
		return command.substr(spaceCount + 1, command.length() - spaceCount - 1);
	}
}
