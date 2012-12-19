#include "Overlay.h"
#include "Root.h"
#include <iostream>
#include "Interface.h"
#include "Fonts.h"
#include "Textures.h"
#include "common/Logger.h"
#include <sstream>
#include <vector>
#define STB_TRUETYPE_IMPLEMENTATION
#include "../ext/stb_truetype.h"
using std::vector;

#define OFFSET_X 30.0f
#define OFFSET_Y 30.0f

namespace Arya
{
    bool Interface::init()
    {
        Font* font = FontManager::shared().getFont("courier.ttf");
        if(!font) return false;
        for(int i = 0; i < 9; i++)
        {
            Rect* rect = new Rect;
            rects.push_back(rect);
            rects[i]->textureHandle = font->textureHandle;
            rects[i]->offsetInPixels.y = OFFSET_Y;
            Root::shared().getOverlay()->addRect(rect);
        }

        float xpos = 0.0f, ypos = 0.0f;
        stbtt_aligned_quad q;
        string s = "FPS = ";

        float lastX = 0.0f;

        for(unsigned int i = 0; i < s.size(); i++)
        {
            stbtt_GetBakedQuad(font->baked, 512, 512, s[i], &xpos ,&ypos,&q,true);
            rects[i]->texOffset = vec2(q.s0, 1 - q.t0 - (q.t1 - q.t0));
            rects[i]->texSize = vec2(q.s1 - q.s0, (q.t1 - q.t0));
            rects[i]->offsetInPixels.x = OFFSET_X + lastX;
            rects[i]->sizeInPixels = vec2(q.x1 - q.x0, (q.y1 - q.y0));
            rects[i]->offsetInPixels.y = OFFSET_Y - (rects[i]->sizeInPixels.y/2.0);

            lastX = xpos;
        }

        offsetFPS = lastX;

        return true;
    }

    void Interface::onFrame(float elapsedTime)
    {	
        Font* font = FontManager::shared().getFont("courier.ttf");
        time += elapsedTime;
        count += 1;

        if (time >= 1.0)
        {
            float xpos = 0.0f, ypos = 0.0f;
            std::stringstream myStream;
            myStream.fill('0');
            myStream.width(3);
            if(count >= 1000) count = 999;
            myStream << count;
            stbtt_aligned_quad q;
            float lastX = 0.0f;
            for(int i = 0; i < 3; i++)
            {
                stbtt_GetBakedQuad(font->baked, 512, 512, myStream.str()[i], &xpos ,&ypos,&q,true);
                rects[i + 6]->texOffset = vec2(q.s0, 1 - q.t0 - (q.t1 - q.t0));
                rects[i + 6]->texSize = vec2(q.s1 - q.s0, (q.t1 - q.t0));
                rects[i + 6]->sizeInPixels = vec2(q.x1 - q.x0, (q.y1 - q.y0));
                rects[i + 6]->offsetInPixels.x = lastX + offsetFPS + OFFSET_X;
                rects[i + 6]->offsetInPixels.y = OFFSET_Y - (rects[i + 6]->sizeInPixels.y / 2.0);
                lastX = xpos;
            }
            count = 0;
            time = 0.0;
        }
    }
}
