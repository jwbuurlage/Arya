#include "Overlay.h"
#include "Fonts.h"
#include "Root.h"
#include "Textures.h"
#include "Shaders.h"
#include "common/Logger.h"

namespace Arya
{
    Overlay::Overlay(ShaderProgram* _overlayProgram, GLuint _rectVAO) 
    {
        rectVAO = _rectVAO;
        overlayProgram = _overlayProgram;

		ww = 0;
		wh = 0;
    }

    Overlay::~Overlay()
    {
        for(vector<Rect*>::iterator it = rects.begin();
                it != rects.end(); ++it)
            delete *it;
        rects.clear();
    }

    bool Overlay::init()
    {
        ww = Root::shared().getWindowWidth();
        wh = Root::shared().getWindowHeight();

        return true;
    }

    Rect* Overlay::createRect()
    {
        Rect* newRect = new Rect;
        rects.push_back(newRect);
        return newRect;
    }

    void Overlay::removeRect(Rect* rect)
    {
        for(vector<Rect*>::iterator it = rects.begin(); it != rects.end();)
        {
            if( *it == rect ) it = rects.erase(it);
            else ++it;
        }
        delete rect;
    }

    void Overlay::render()
    {
        // bind shader
        overlayProgram->use();
        glBindVertexArray(rectVAO);

        overlayProgram->setUniform1i("texture1", 0);
        glActiveTexture(GL_TEXTURE0);

        // render all rects
        for(unsigned int i = 0; i < rects.size(); ++i)
        {
            if(!(rects[i]->isVisible)) continue;
            overlayProgram->setUniform4fv("uColor", rects[i]->fillColor);
            if(rects[i]->textureHandle !=0)
            {
                glBindTexture(GL_TEXTURE_2D, rects[i]->textureHandle);
            }
            else
            {
                glBindTexture(GL_TEXTURE_2D, TextureManager::shared().getTexture("white")->handle);
            }
            // TODO: use dirty flag maybe?
            rects[i]->screenPosition = rects[i]->relative + vec2(2.0 * rects[i]->offsetInPixels.x / ww, 2.0 * rects[i]->offsetInPixels.y / wh);
            rects[i]->screenSize = vec2(2.0 * rects[i]->sizeInPixels.x / ww, 2.0 * rects[i]->sizeInPixels.y / wh);
            overlayProgram->setUniform2fv("screenSize", rects[i]->screenSize);
            overlayProgram->setUniform2fv("screenPosition", rects[i]->screenPosition);
            overlayProgram->setUniform2fv("texOffset", vec2(rects[i]->texOffset.x, 1 - rects[i]->texOffset.y));
            overlayProgram->setUniform2fv("texSize", vec2(rects[i]->texSize.x, -rects[i]->texSize.y));
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }
}
