#include "Console.h"
#include "Root.h"
#include "Overlay.h"
#include "Fonts.h"
#include "Config.h"
#include "Sounds.h"
#include "common/Logger.h"
#include <sstream>
using std::stringstream;

namespace Arya
{
    template<> Console* Singleton<Console>::singleton = 0;

    Console::Console()
    {
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
    };

    Console::~Console()
    {
        cleanup();
    }

    bool Console::init()
    {
        if((!Root::shared().getWindowWidth()) || (!Root::shared().getWindowHeight())) return false;
        nrCharOnLine = (Root::shared().getWindowWidth()-(Root::shared().getWindowWidth() % textWidthInPixels))/textWidthInPixels;
        font = FontManager::shared().getFont("courier.ttf"); //font to be used
        if(!font) return false;

        Rect* rect = new Rect; // here we initialize the frame for the console itself
        rects.push_back(rect);
        rect->offsetInPixels.x = 0.0;
        rect->offsetInPixels.y = Root::shared().getWindowHeight() - heightKernel;
        rect->sizeInPixels.x = Root::shared().getWindowWidth() + 0.0;
        rect->sizeInPixels.y = heightKernel; // height of kernel in pixels
        rect->fillColor = vec4(0.0, 0.0, 0.0, 0.4);
        rect->isVisible = false;
        Root::shared().getOverlay()->addRect(rect);

        for(int i = 0; i < nrLines; i++) // next, we add the rectangles for the history, this will be changed into sentences instead of seperate characters
        {
            for(int j = 0; j < nrCharOnLine; j++)
            {
                Rect* rect = new Rect;
                rects.push_back(rect);
                rect->sizeInPixels.x = textWidthInPixels;
                rect->sizeInPixels.y = textHeightInPixels;
                rect->offsetInPixels.x = j * rect->sizeInPixels.x;
                rect->offsetInPixels.y = Root::shared().getWindowHeight() - (i+1) * (textHeightInPixels + pixelsInBetween);
                rect->textureHandle = font->textureHandle;
                rect->isVisible = false;
                Root::shared().getOverlay()->addRect(rect);
            }
        }

        for(int i = 0; i < nrCharOnLine; i++) // here, we add rectangles for the currentLine. These should be seperate, because we need to be able to delete and add letters
        {
            Rect* rect = new Rect;
            rects.push_back(rect);
            rect->sizeInPixels.x = textWidthInPixels;
            rect->sizeInPixels.y = textHeightInPixels;
            rect->offsetInPixels.x = i * rect->sizeInPixels.x;
            rect->offsetInPixels.y = Root::shared().getWindowHeight() - (history.size() + 1) * (rect->sizeInPixels.y + pixelsInBetween);
            rect->isVisible = false;
            Root::shared().getOverlay()->addRect(rect);
        }
        addCommandListener("consoleColor", this);
        addCommandListener("hide", this);
        addCommandListener("getVarValue", this);
        addCommandListener("setVarValue", this);
        addCommandListener("PLAYSOUND", this);
        addCommandListener("PLAYMUSIC", this);
        addCommandListener("STOPSOUND", this);
        addCommandListener("STOPMUSIC", this);
        return true;
    }

    bool Console::handleCommand(string command)
    {
        bool flag = true;
        if(splitLineCommand(command) == "consoleColor")
        {
            float float1 = 0.0f;
            float float2 = 0.0f;
            float float3 = 0.0f;
            stringstream parser;
            parser << splitLineParameters(command);
            parser >> float1 >> float2 >> float3;
            changeConsoleColor(float1, float2, float3);
            if(Root::shared().getConfigIsInit()) Config::shared().editConfigFile(command);
        }
        if(splitLineCommand(command) == "hide")
        {
            if(splitLineParameters(command) == "console") toggleVisibilityConsole();
        }
        if(splitLineCommand(command) == "get")
        {
            string variableName;
            stringstream parser;
            parser << splitLineParameters(command);
            parser >> variableName;
            //string output = Config::shared().getVarValue(variableName);
            //LOG_INFO("The value of " << variableName << " is: " << output);
        }
        if(splitLineCommand(command) == "set")
        {
            string variableName;
            string value;
            stringstream parser;
            parser << splitLineParameters(command);
            parser >> variableName >> value;

           // string output = Config::shared().getVarValue(variableName);
           // LOG_INFO("The value of " << variableName << " is: " << output);
           // Config::shared().setVarValue(variableName, value);
           // string output1 = Config::shared().getVarValue(variableName);
           // LOG_INFO("The value of " << variableName << " is: " << output1);
        }
        int count = 0;
        if(splitLineCommand(command) == "PLAYSOUND")
        {
            count = SoundManager::shared().play("testSound.wav");
            SoundManager::shared().setLoopSound("testSound.wav", count, 0.01, true);
            if(count == -1000) LOG_WARNING("SoundFile testSound.wav not found!");
        }
        if(splitLineCommand(command) == "PLAYMUSIC")
        {
            if(SoundManager::shared().play("testMusic.wav") == -1000) LOG_WARNING("MusicFile testMusic.wav not found!");
        }
        if(splitLineCommand(command) == "STOPMUSIC")
        {
            SoundManager::shared().stopMusic("testMusic.wav");
        }
        if(splitLineCommand(command) == "STOPSOUND")
        {
            SoundManager::shared().setLoopSound("testSound.wav", count, 0.5, false);
            SoundManager::shared().stopSound("testSound.wav", count, 0.001);
        }
        else flag = false;
        return flag;
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
            else if(key >= GLFW_KEY_KP_0  && key <= GLFW_KEY_KP_9  && keyDown && (currentLine.length() < (unsigned)nrCharOnLine)) currentLine.push_back(key - 2);
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
                rects[nrLines * nrCharOnLine + j + 1]->offsetInPixels.y = Root::shared().getWindowHeight() - (history.size() + 1) * (textHeightInPixels + pixelsInBetween);
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
        for(int i = 0; (unsigned)i < rects.size(); i++)
        {
            Root::shared().getOverlay()->removeRect(rects[i]);
        }
    }

    void Console::setVisibilityConsole(bool flag) //set the visibility of the console
    {
        visibility = flag;
    }

    void Console::toggleVisibilityConsole() //toggle visibility of the console
    {
        if(visibility) visibility = false;
        else visibility = true;
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
        onCommand(currentLine);
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
            for(int i = 1; (textToBeAdded.length() > (i * nrCharOnLine)); i++)
                count = i;

            for(int i = 0; i < count - 2; i++)
            {
                history.push_back(textToBeAdded.substr(i * nrCharOnLine, (i + 1)* nrCharOnLine));
            }
            history.push_back(textToBeAdded.substr((count - 1) * nrCharOnLine, (textToBeAdded.length() % nrCharOnLine)));
        }
        if(history.size() == (unsigned)nrLines - 1) history.erase(history.begin());
    }


    void Console::addCommandListener(string command, CommandListener* listener)
    {
        commandListeners.insert(make_pair(command, listener));
    }

    void Console::removeCommandListener(string command, CommandListener* listener)
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

    void Console::onCommand(string command) // alleen op eerste woord zoeken
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

    bool Console::changeConsoleColor(float r, float g, float b)
    {
        bool flag = true;
        if( r >= 0.0f && r <= 1.0f && g >= 0.0f && g <= 1.0f && b >= 0.0f && g <= 1.0f ) rects[0]->fillColor = vec4(r,g,b,0.4f);
        else flag = false;
        return flag;
    }

    string Console::splitLineCommand(string command)
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

    string Console::splitLineParameters(string command)
    {
        int spaceFinder = 0;
        spaceFinder = command.find(" ", 0);
        if((unsigned)spaceFinder == std::string::npos)
        {
            return "";
        }
        int spaceCount = command.find_first_of(" ", 0);
        return command.substr(spaceCount + 1, currentLine.length() - spaceCount - 1);
    }
}
