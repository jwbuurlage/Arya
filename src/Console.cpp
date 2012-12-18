#include "Console.h"
#include "Root.h"
#include "Overlay.h"
#include "Fonts.h"
#include "common/Logger.h"
namespace Arya
{
    template<> Console* Singleton<Console>::singleton = 0;

    Console::Console()
    {
        nrLines = 20;
        searchNrLines = 50;
        textWidthInPixels = 10;
        textHeightInPixels = 10.0;
        pixelsInBetween = 4.0;
        visibility = false;
        activeLine = 0;
        nrCharOnLine = 0;
        font = 0;
        time = 0.0;
        lShift = false;
        rShift = false;
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
        rect->offsetInPixels.y = Root::shared().getWindowHeight() - 270.0;
        rect->sizeInPixels.x = Root::shared().getWindowWidth() + 0.0;
        rect->sizeInPixels.y = 270.0; // height of kernel in pixels
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
        return true;
    }

    bool Console::keyDown(int key, bool keyDown)
    {
        bool keyHandled = true;
        if(visibility == false)
        {
            switch(key)
            {
                case GLFW_KEY_F6: if(keyDown) toggleVisibilityConsole(); break;
                default: keyHandled = false; break;
            }
        }
        else
        {
            if(key >= 'A' && key <= 'Z')
            {
                if((rShift && !cLock) || (lShift && !cLock) || (cLock && !lShift) || (cLock && !rShift))
                {
                    if(keyDown && (currentLine.length() < nrCharOnLine)) currentLine.push_back(key);
                }
                else
                {
                    if((keyDown && currentLine.length() < nrCharOnLine)) currentLine.push_back(key - 'A' + 'a');
                }
            }
            else
            {
                switch(key)
                {
                    case GLFW_KEY_ESC:
                    case GLFW_KEY_F6: if(keyDown) toggleVisibilityConsole(); break;
                    case GLFW_KEY_ENTER: if(keyDown) enterInput(); break;
                    case GLFW_KEY_BACKSPACE: if(keyDown && currentLine.length() > 0)
                                             {
                                                 currentLine.erase(currentLine.end()-1);
                                             }; break;
                    case GLFW_KEY_SPACE: if(keyDown) currentLine.push_back(key); break;
                    case GLFW_KEY_RSHIFT: if(!rShift) rShift = true; else rShift = false; break;
                    case GLFW_KEY_LSHIFT: if(!lShift) lShift = true; else lShift = false; break;
                    case GLFW_KEY_CAPS_LOCK: if(keyDown)
                                             {
                                                 if(cLock) cLock = false;
                                                 else cLock = true;
                                             }; break;
                    case GLFW_KEY_UP: if(keyDown && searchHistory.size() > upCount)
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
                for(int i = 0; (i < nrLines && i < history.size()); i++) // fills the rectangles that need to be filled
                {
                    for(int j = 0; j < history[i].length(); j++)
                    {
                        rects[i*nrCharOnLine + j + 1]->isVisible = visibility; // + 1 because of the console rect
                        stbtt_GetBakedQuad(font->baked, 512, 512, history[i].at(j),&xpos ,&ypos,&q,true);
                        rects[i*nrCharOnLine + j + 1]->texOffset = vec2(q.s0, 1 - q.t0 - (q.t1 - q.t0));
                        rects[i*nrCharOnLine + j + 1]->texSize = vec2(q.s1 - q.s0, (q.t1 - q.t0));
                    }
                }
                for(int i = 0; (i < nrLines && i < history.size()); i++) // clears the fonttextures of the rectangles that do not need to be filled
                {
                    for(int j = 0; j < nrCharOnLine; j++)
                    {
                        if(history[i].length() <= j) rects[i*nrCharOnLine + j + 1]->isVisible = false;
                    }
                }
            }
            for(int j = 0; j < currentLine.length(); j++)
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
                if(currentLine.size() <= j) rects[nrLines * nrCharOnLine + j + 1]->isVisible = false;
            }
        }
        else time = 0.0;
    }

    void Console::cleanup()
    {
        for(int i = 0; i < rects.size(); i++)
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
        activeLine += 1;
        if(history.size() == nrLines - 1) history.erase(history.begin());
        if(searchHistory.size() == searchNrLines) searchHistory.erase(searchHistory.begin());
    }

    void Console::enterInput() // When you press enter, currentLine needs to be emptied and needs to be added to the history and searchhistory
    {
        addTextLine(currentLine);
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
        LOG_INFO("size of searchhistory is: " << searchHistory.size());
    }

}
