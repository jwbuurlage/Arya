#include "Overlay.h"
#include "Root.h"
#include <iostream>
#include "Interface.h"
#include "Fonts.h"
#include "common/Logger.h"
#include <sstream>
#include <vector>
#define STB_TRUETYPE_IMPLEMENTATION
#include "../ext/stb_truetype.h"
using std::vector;

namespace Arya
{
  float time = 0.0; 
  float count = 0.0;

  void Interface::init()
  {
    vector<Rect*> rects;
    for(int i = 0; i<12; i++)
    {
      Rect* rect = new Rect;
      rects.push_back(rect);
      rect->offsetInPixels.x = i * 16.0;
      rect->offsetInPixels.y = 0.0;
      rect->sizeInPixels.x = 16.0;
      rect->sizeInPixels.y = 16.0;
      Root::shared().getOverlay()->addRect(rect);
    }
  }
  void Interface::onFrame(float elapsedTime)
  {	
    Font *font = new Font;
    font = FontManager::shared().getFont("courier.ttf");
    time += elapsedTime;
    count += 1.0;

    if (time >= 1.0)
    {
      float xpos = 0.0f, ypos = 0.0f;      
      std::stringstream myStream;
      stbtt_aligned_quad q;
      myStream << "FPS = " << count;
      for(int i = 0; (i < 12 && i < myStream.str().size()); i++)
      {
        stbtt_GetBakedQuad(font->baked, 512, 512, myStream.str()[i], &xpos ,&ypos,&q,true);
      }
      count = 0.0;
      time = 0.0;
    }
  }
}
