#include "Console.h"
#include "Commands.h"
#include "Root.h"
#include "Overlay.h"
#include "Fonts.h"
#include "Config.h"
#include "Sounds.h"
#include "Interface.h"
#include "Textures.h"
#include "common/Logger.h"
#include <sstream>
using namespace std;
using std::stringstream;

namespace Arya
{
	template<> Console* Singleton<Console>::singleton = 0;

	void LogCallback(const std::string &logmsg)
	{
		if( &Console::shared() && Console::shared().isInitialized() )
			Console::shared().addOutputText(logmsg);
	}

	Console::Console()
	{
		initialized = false;
		nrLines = 20;
		searchNrLines = 50;
		textWidthInPixels = 10;
		pixelsInBetween = 4.0;
		visibility = false;
		font = 0;
		lShift = false;
		rShift = false;
		rAlt = false;
		cLock = false;
		upCount = 0;
		consoleColor= vec4(0,0,0,0.4);
        consoleWindow = 0;

		CommandHandler::shared().addCommandListener("consoleColor", this);
		CommandHandler::shared().addCommandListener("hide", this);
	};

	Console::~Console()
	{
		CommandHandler::shared().removeCommandListener(this);
		cleanup();

        if(consoleWindow)
            delete consoleWindow;
	}

	bool Console::init()
	{
		if(initialized) return true;

		font = FontManager::shared().getFont("courier.ttf"); //font to be used
		if(!font) return false;

		Logger::shared().setLoggerCallback(LogCallback);

		// Console window
		vec2 consoleWindowSize = vec2(400.0f, 600.0f);
		consoleWindow = new Window(
				vec2(-1.0f, 1.0f), 
				vec2(10.0f, -consoleWindowSize.y - 10.0f), 
				consoleWindowSize,
				TextureManager::shared().getTexture("white"), 
				WINDOW_DRAGGABLE | WINDOW_RESIZABLE | WINDOW_CLOSABLE, 
				"Console",
				consoleColor);
		Interface::shared().makeInactive(consoleWindow);
		addLine("");

		initialized = true;
		return true;
	}

	bool Console::keyDown(int key, bool keyDown)
	{
		int nrCharOnLine = (Root::shared().getWindowWidth()-(Root::shared().getWindowWidth() % textWidthInPixels))/textWidthInPixels;
		bool keyHandled = true;
		if(visibility == false)
		{
			glfwDisable(GLFW_KEY_REPEAT);
			switch(key)
			{
				case GLFW_KEY_F6: if(keyDown) 
								  {
									  glfwEnable(GLFW_KEY_REPEAT);
									  toggleVisibilityConsole();
								  }
								  break;
				default: keyHandled = false; break;
			}
		}
		else
		{
			if(key >= 'A' && key <= 'Z')
			{
				if((rShift && !cLock) || (lShift && !cLock) || (cLock && !lShift) || (cLock && !rShift))
				{
					if(keyDown && (currentLine.length() < (unsigned)nrCharOnLine))
					{
						currentLine.push_back(key);
						lines[0]->setText(currentLine);
					}
				}
				else
				{
					if((keyDown && currentLine.length() < (unsigned)nrCharOnLine)) 
					{
						currentLine.push_back(key - 'A' + 'a');
						lines[0]->setText(currentLine);
					}
				}
			}
			else if(key >= '0' && key <= '9' && keyDown && (currentLine.length() < (unsigned)nrCharOnLine)) 
			{
				currentLine.push_back(key);
				lines[0]->setText(currentLine);
			}
			else if(key >= GLFW_KEY_KP_0  && key <= GLFW_KEY_KP_9  && keyDown && (currentLine.length() < (unsigned)nrCharOnLine)) 
			{
				currentLine.push_back(key+2);
				lines[0]->setText(currentLine);
			}
			else if (keyDown && key == GLFW_KEY_KP_ADD && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == GLFW_KEY_KP_SUBTRACT && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == GLFW_KEY_KP_MULTIPLY && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == GLFW_KEY_KP_DIVIDE && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == GLFW_KEY_KP_EQUAL && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == GLFW_KEY_KP_DECIMAL && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == '.' && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == ',' && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == ':' && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == '=' && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == '/' && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == ';' && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == '?' && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == '!' && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == '$' && !rAlt && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && key == '^' && !rAlt && currentLine.length() < (unsigned)nrCharOnLine) LOG_INFO(key);
			else if (keyDown && (key == '(' || key == ')') && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else if (keyDown && ((key == ']') || (key == '$' && rAlt )) && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(']');
			else if (keyDown && ((key == '[') || (key == '^' && rAlt )) && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back('[');
			else if (keyDown && (key == '{' || key == '}') && currentLine.length() < (unsigned)nrCharOnLine) currentLine.push_back(key);
			else
			{
				switch(key)
				{
					case GLFW_KEY_ESC:
					case GLFW_KEY_F6: if(keyDown) toggleVisibilityConsole(); break;
					case GLFW_KEY_KP_ENTER:
					case GLFW_KEY_ENTER: if(keyDown) enterInput(); break;
					case GLFW_KEY_BACKSPACE: if(keyDown && currentLine.length() > 0)
											 {
												 currentLine.erase(currentLine.end()-1);
												 lines[0]->setText(currentLine);
											 }; break;
					case GLFW_KEY_SPACE: if(keyDown) currentLine.push_back(key); break;
					case GLFW_KEY_RSHIFT: if(!rShift) rShift = true; else rShift = false; break;
					case GLFW_KEY_LSHIFT: if(!lShift) lShift = true; else lShift = false; break;
					case GLFW_KEY_RALT: if(!rAlt) rAlt = true; else rAlt = false; break;
					case GLFW_KEY_CAPS_LOCK: if(keyDown)
											 {
												 if(cLock) cLock = false;
												 else cLock = true;
											 }; break;
					case GLFW_KEY_UP: if(keyDown && searchHistory.size() > (unsigned)upCount)
									  {
										  upCount += 1;
										  goBackInHistory();
									  }; break;
					case GLFW_KEY_DOWN: if(keyDown && 0 < upCount)
										{
											upCount -= 1;
											goBackInHistory();
										}; break;
					default: keyHandled = false; break;
				}
			}
		}
		return keyHandled;
	}

	void Console::onFrame(float elapsedTime)
	{
		lines[0]->setText(currentLine);
	}

	void Console::cleanup()
	{
		Logger::shared().setLoggerCallback(0);
	}

	void Console::toggleVisibilityConsole() //toggle visibility of the console
	{
		visibility = !visibility;

		if(consoleWindow->getActiveState())	Interface::shared().makeInactive(consoleWindow);
		else Interface::shared().makeActive(consoleWindow);
	}

	void Console::addTextLine(string textToBeAdded) //add line of text to the history (as well as to the search history). If nr of lines > maxnr then the first one will be deleted
	{
		history.push_back(textToBeAdded);
		searchHistory.push_back(textToBeAdded);
		if(history.size() == (unsigned)nrLines - 1) history.erase(history.begin());
		if(searchHistory.size() == (unsigned)searchNrLines) searchHistory.erase(searchHistory.begin());
	}

	void Console::enterInput() // When you press enter, currentLine needs to be emptied and needs to be added to the history and searchhistory
	{
		addLine(currentLine);
		addTextLine(currentLine);
		CommandHandler::shared().onCommand(currentLine);
		for(int i = 0; ((unsigned)i < history.size()); i++)
		{
			lines[i+1]->setText(history[history.size()-1-i]);
		}
		upCount = 0;
		currentLine = "";
		lines[0]->setText(currentLine);
	}

	void Console::goBackInHistory()
	{
		int count = 0;
		for(int j = 0; j < upCount; j++)
		{
			count = j;
			currentLine = searchHistory[searchHistory.size()- 1 - count];
		}
	}

	void Console::addOutputText(string textToBeAdded)
	{
		int nrCharOnLine = (Root::shared().getWindowWidth()-(Root::shared().getWindowWidth() % textWidthInPixels))/textWidthInPixels;
		addLine(textToBeAdded);
		if(textToBeAdded.length() <= (unsigned)nrCharOnLine) 
		{
			history.push_back(textToBeAdded);
		}
		else
		{
			int count = 0;
			for(unsigned int i = 1; (textToBeAdded.length() > (i * nrCharOnLine)); i++)
				count = i;

			for(int i = 0; i < count - 2; i++)
			{
				history.push_back(textToBeAdded.substr(i * nrCharOnLine, (i + 1)* nrCharOnLine));
			}
			history.push_back(textToBeAdded.substr((count - 1) * nrCharOnLine, (textToBeAdded.length() % nrCharOnLine)));
		}
		if(history.size() == (unsigned)nrLines - 1) history.erase(history.begin());
	}

	bool Console::changeConsoleColor(float r, float g, float b)
	{
		bool flag = true;
		if( r >= 0.0f && r <= 1.0f && g >= 0.0f && g <= 1.0f && b >= 0.0f && b <= 1.0f ) 
		{
			consoleColor = vec4(r,g,b,0.4f);
			if(consoleWindow) 
			{
				consoleWindow->setColorMask(consoleColor);
			}
		}
		else flag = false;
		return flag;
	}
	bool Console::handleCommand(string command)
	{
		bool flag = true;
		if(CommandHandler::shared().splitLineCommand(command) == "consoleColor")
		{
			float float1 = 0.0f;
			float float2 = 0.0f;
			float float3 = 0.0f;
			std::stringstream parser;
			parser << CommandHandler::shared().splitLineParameters(command);
			parser >> float1 >> float2 >> float3;
			changeConsoleColor(float1, float2, float3);
			Config::shared().editConfigFile(command);
		}
		if(CommandHandler::shared().splitLineCommand(command) == "hide")
		{
			if(CommandHandler::shared().splitLineParameters(command) == "console") toggleVisibilityConsole();
		}
		else flag = false;
		return flag;
	}
	void Console::addLine(string text)
	{
		Label* label = new Label(vec2(-1.0f, -1.0f),
				vec2(0.0f, (lines.size()+1)*20.0f),
				font,
				text);
		lines.push_back(label);
		consoleWindow->addChild(lines.back());
	}
}
