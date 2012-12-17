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
    pixelsInBetween = 4.0;
    visibility = false;
    activeLine = 0;
    nrCharOnLine = 0;
    font = 0;
    time = 0.0;
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
    rect->offsetInPixels.y = Root::shared().getWindowHeight() - 280.0;
    rect->sizeInPixels.x = Root::shared().getWindowWidth() + 0.0;
    rect->sizeInPixels.y = 280.0; // height of kernel in pixels
    rect->fillColor = vec4(0.0, 0.0, 0.0, 0.4);
    rect->isVisible = false;
    Root::shared().getOverlay()->addRect(rect);

    for(int i = 0; i < nrLines; i++) // next, we add the rectangles for the history, this will be changed into sentences instead of loose characters
    {
      for(int j = 0; j < nrCharOnLine; j++)
      {
        Rect* rect = new Rect;
        rects.push_back(rect);
        rect->sizeInPixels.x = textWidthInPixels;
        rect->sizeInPixels.y = 10.0;
        rect->offsetInPixels.x = j * rect->sizeInPixels.x;
        rect->offsetInPixels.y = Root::shared().getWindowHeight() - (i + 1) * (rect->sizeInPixels.y + pixelsInBetween);
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
      rect->sizeInPixels.y = 10.0;
      rect->offsetInPixels.x = i * rect->sizeInPixels.x;
      rect->offsetInPixels.y = Root::shared().getWindowHeight() - (history.size() + 1) * (rect->sizeInPixels.y + pixelsInBetween);
      rect->isVisible = false;
      Root::shared().getOverlay()->addRect(rect);
    }
    return true;
  }

  void Console::onFrame(float elapsedTime)
  {
    if(!visibility) return;
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
            rects[(history.size() -i)*nrCharOnLine + j + 1]->isVisible = visibility; // + 1 because of the console rect
            stbtt_GetBakedQuad(font->baked, 512, 512, history[i].at(j),&xpos ,&ypos,&q,true);
            rects[(history.size() -i)*nrCharOnLine + j + 1]->texOffset = vec2(q.s0, 1 - q.t0 - (q.t1 - q.t0));
            rects[(history.size() -i)*nrCharOnLine + j + 1]->texSize = vec2(q.s1 - q.s0, (q.t1 - q.t0));
          }
        }
        for(int i = 0; (i < nrLines && i < history.size()); i++) // clears the fonttextures of the rectangles that do not need to be filled
        {
          for(int j = 0; j < nrCharOnLine; j++)
          {
            if(history[i].length() < j) rects[(history.size() - i)*nrCharOnLine + j + 1]->isVisible = false;
          }
        }
      }
      if(currentLine.empty() == false)
      {
        for(int j = 0; j < currentLine.length(); j++)
        {
          rects[nrLines * nrCharOnLine + j + 1]->textureHandle = font->textureHandle;
          rects[nrLines * nrCharOnLine + j + 1]->isVisible = visibility; // + 1 because of the console rect
          stbtt_GetBakedQuad(font->baked, 512, 512, currentLine.at(j),&xpos ,&ypos,&q,true);
          rects[nrLines * nrCharOnLine + j + 1]->texOffset = vec2(q.s0, 1 - q.t0 - (q.t1 - q.t0));
          rects[nrLines * nrCharOnLine + j + 1]->texSize = vec2(q.s1 - q.s0, (q.t1 - q.t0));
        }
        for(int j = 0; j < nrCharOnLine; j++)
        {
          if(currentLine.size() <= j)rects[nrLines * nrCharOnLine + j + 1]->isVisible = false;
        }
      }
    }
    else time = 0.0;
  }

  void Console::cleanup(){}

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
    if(history.size() ==  nrLines) history.erase(history.begin());
    if(searchHistory.size() == searchNrLines) searchHistory.erase(searchHistory.begin());
  }

  void Console::enterInput() // When you press enter, currentLine needs to be emptied and needs to be added to the history and searchhistory
  {
    addTextLine(currentLine);
    currentLine = "";
  }
}
