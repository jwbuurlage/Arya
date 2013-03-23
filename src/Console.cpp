#include "Console.h"
#include "Commands.h"
#include "Root.h"
#include "Overlay.h"
#include "Fonts.h"
#include "Config.h"
#include "Sounds.h"
#include "Interface.h"
#include "common/Logger.h"
#include <sstream>
using std::stringstream;

//consoleWindow = new Window();
//interface->addWindow(consoleWindow);
//// initialize Window
//
//consoleWindow->setSize(..);
//consoleWindow->setPosition(..);
//
//for(i in history....)
//{
//	consoleWindow->addChild(Label("courier", history[i], position, size));
//}
//
//interface->makeActive(consoleWindow);

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
        heightKernel = 270.0;
        textWidthInPixels = 10;
        textHeightInPixels = 10.0;
        pixelsInBetween = 4.0;
        visibility = false;
        nrCharOnLine = 0;
        font = 0;
        time = 0.0;
        lShift = false;
        rShift = false;
        rAlt = false;
        cLock = false;
        upCount = 0;
        consoleColor= vec4(0,0,0,0);

		CommandHandler::shared().addCommandListener("consoleColor", this);
		CommandHandler::shared().addCommandListener("hide", this);
    };

    Console::~Console()
    {
        CommandHandler::shared().removeCommandListener(this);
        cleanup();
    }

    bool Console::init()
    {
        if(initialized) return true;

        if((!Root::shared().getWindowWidth()) || (!Root::shared().getWindowHeight())) return false;
        nrCharOnLine = (Root::shared().getWindowWidth()-(Root::shared().getWindowWidth() % textWidthInPixels))/textWidthInPixels;
        font = FontManager::shared().getFont("courier.ttf"); //font to be used
        if(!font) return false;


        Rect* rect = Root::shared().getOverlay()->createRect(); // here we initialize the frame for the console itself
        rects.push_back(rect);
        rect->offsetInPixels.x = 0.0;
        rect->offsetInPixels.y = Root::shared().getWindowHeight() - heightKernel;
        rect->sizeInPixels.x = Root::shared().getWindowWidth() + 0.0;
        rect->sizeInPixels.y = heightKernel; // height of kernel in pixels
        rect->fillColor = vec4(0.0, 0.0, 0.0, 0.4);
        rect->isVisible = false;

        for(int i = 0; i < nrLines; i++) // next, we add the rectangles for the history, this will be changed into sentences instead of seperate characters
        {
            for(int j = 0; j < nrCharOnLine; j++)
            {
                Rect* rect = Root::shared().getOverlay()->createRect();
                rects.push_back(rect);
                rect->sizeInPixels.x = textWidthInPixels;
                rect->sizeInPixels.y = textHeightInPixels;
                rect->offsetInPixels.x = j * rect->sizeInPixels.x;
                rect->offsetInPixels.y = Root::shared().getWindowHeight() - (i+1) * (textHeightInPixels + pixelsInBetween);
                rect->textureHandle = font->textureHandle;
                rect->isVisible = false;
            }
        }

        for(int i = 0; i < nrCharOnLine; i++) // here, we add rectangles for the currentLine. These should be seperate, because we need to be able to delete and add letters
        {
            Rect* rect = Root::shared().getOverlay()->createRect();
            rects.push_back(rect);
            rect->sizeInPixels.x = textWidthInPixels;
            rect->sizeInPixels.y = textHeightInPixels;
            rect->offsetInPixels.x = i * rect->sizeInPixels.x;
            rect->offsetInPixels.y = Root::shared().getWindowHeight() - (history.size() + 1) * (rect->sizeInPixels.y + pixelsInBetween);
            rect->isVisible = false;
        }
        Logger::shared().setLoggerCallback(LogCallback);

        initialized = true;
        return true;
    }

    bool Console::keyDown(int key, bool keyDown)
    {
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
                    if(keyDown && (currentLine.length() < (unsigned)nrCharOnLine)) currentLine.push_back(key);
                }
                else
                {
                    if((keyDown && currentLine.length() < (unsigned)nrCharOnLine)) currentLine.push_back(key - 'A' + 'a');
                }
            }
            else if(key >= '0' && key <= '9' && keyDown && (currentLine.length() < (unsigned)nrCharOnLine)) currentLine.push_back(key);
            else if(key >= GLFW_KEY_KP_0  && key <= GLFW_KEY_KP_9  && keyDown && (currentLine.length() < (unsigned)nrCharOnLine)) currentLine.push_back(key+2);
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
        time += elapsedTime;
        if(time < 2.0) // build something in for moving cursor, but need to wait with implementation untill I added some more keyboard commands.
        {
            float xpos = 0.0f, ypos = 0.0f;
            stbtt_aligned_quad q;
            rects[0]->isVisible = visibility;
            rects[0]->fillColor = consoleColor;
            if(history.empty() == false)
            {
                for(int i = 0; (i < nrLines && (unsigned)i < history.size()); i++) // fills the rectangles that need to be filled
                {
                    for(int j = 0; (unsigned)j < history[i].length(); j++)
                    {
                        rects[i*nrCharOnLine + j + 1]->isVisible = visibility; // + 1 because of the console rect
                        stbtt_GetBakedQuad(font->baked, 512, 512, history[i].at(j),&xpos ,&ypos,&q,true);
                        rects[i*nrCharOnLine + j + 1]->texOffset = vec2(q.s0, 1 - q.t0 - (q.t1 - q.t0));
                        rects[i*nrCharOnLine + j + 1]->texSize = vec2(q.s1 - q.s0, (q.t1 - q.t0));
                        rects[i*nrCharOnLine + j + 1]->offsetInPixels.y = Root::shared().getWindowHeight() - (i+1) * (textHeightInPixels + pixelsInBetween) - q.y1;
                        rects[i*nrCharOnLine + j + 1]->sizeInPixels = vec2(q.x1 - q.x0, (q.y1 - q.y0));
                    }
                }
                for(int i = 0; (i < nrLines && (unsigned)i < history.size()); i++) // clears the fonttextures of the rectangles that do not need to be filled
                {
                    for(int j = 0; j < nrCharOnLine; j++)
                    {
                        if(history[i].length() <= (unsigned)j) rects[i*nrCharOnLine + j + 1]->isVisible = false;
                    }
                }
            }
            for(int j = 0; (unsigned)j < currentLine.length(); j++)
            {
                rects[nrLines * nrCharOnLine + j + 1]->textureHandle = font->textureHandle;
                rects[nrLines * nrCharOnLine + j + 1]->isVisible = visibility; // + 1 because of the console rect
                stbtt_GetBakedQuad(font->baked, 512, 512, currentLine.at(j),&xpos ,&ypos,&q,true);
                rects[nrLines * nrCharOnLine + j + 1]->offsetInPixels.y = Root::shared().getWindowHeight() - (history.size() + 1) * (textHeightInPixels + pixelsInBetween) - q.y1;
                rects[nrLines * nrCharOnLine + j + 1]->sizeInPixels = vec2(q.x1 - q.x0, (q.y1 - q.y0));
                rects[nrLines * nrCharOnLine + j + 1]->texOffset = vec2(q.s0, 1 - q.t0 - (q.t1 - q.t0));
                rects[nrLines * nrCharOnLine + j + 1]->texSize = vec2(q.s1 - q.s0, (q.t1 - q.t0));
            }
            for(int j = 0; j < nrCharOnLine; j++)
            {
                if(currentLine.size() <= (unsigned)j) rects[nrLines * nrCharOnLine + j + 1]->isVisible = false;
            }
        }
        else time = 0.0;
    }

    void Console::cleanup()
    {
        Logger::shared().setLoggerCallback(0);

        for(unsigned int i = 0; i < rects.size(); i++)
        {
            Root::shared().getOverlay()->removeRect(rects[i]);
        }
        rects.clear();
    }

    void Console::setVisibilityConsole(bool flag) //set the visibility of the console
    {
        visibility = flag;
    }

    void Console::toggleVisibilityConsole() //toggle visibility of the console
    {
        visibility = !visibility;
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
        addTextLine(currentLine);
        CommandHandler::shared().onCommand(currentLine);
        upCount = 0;
        currentLine = "";
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
        if(textToBeAdded.length() <= (unsigned)nrCharOnLine) history.push_back(textToBeAdded);
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
        if( r >= 0.0f && r <= 1.0f && g >= 0.0f && g <= 1.0f && b >= 0.0f && b <= 1.0f ) consoleColor = vec4(r,g,b,0.4f);
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
}
