#pragma once 

#include <vector>
#include <GL/glew.h>
#include "common/Logger.h"
#include <glm/glm.hpp>

using std::vector;
using glm::vec2;
using glm::vec4;

namespace Arya
{
    class ShaderProgram;

    struct Rect
    {
        //The rect constructor is made private
        //so that only Overlay can create rects
        //If you want to create a Rect you HAVE
        //to use Root::shared().getOverlay()->createRect();
        private:
            friend class Overlay;
            Rect()
            {
                relative = vec2(-1.0);
                offsetInPixels = vec2(0.0);
                sizeInPixels = vec2(0.0);

                fillColor = vec4(1.0);
                borderColor = vec4(0.0);

                textureHandle = 0;
                texSize = vec2(1.0,1.0);
                texOffset = vec2(0.0);

                screenPosition = vec2(0.0);

                isVisible = true; 
            };
            ~Rect() {}

        public:
            vec2 relative; // in screen coordinates [-1, 1]
            vec2 offsetInPixels; // in pixels
            vec2 sizeInPixels; // in pixels

            vec4 fillColor; // in RGBA
            vec4 borderColor; // in RGBA

            GLuint textureHandle;
            vec2 texOffset; // in texcoo [0, 1]
            vec2 texSize; // in texcoo [0, 1]

            vec2 screenPosition; //gets set by Overlay
            vec2 screenSize; //gets set by Overlay

            bool isVisible;
    };

    class Overlay
    {
        public:
            Overlay(ShaderProgram* _overlayProgram, GLuint _rectVAO);
            ~Overlay();

            bool init();

            //Will create a rect and add it
            //to the list. The rect will
            //be deleted at ~Overlay and at
            //removeRect
            Rect* createRect();
            void removeRect(Rect* quad);

            void render();

        private:
            vector<Rect*> rects;
            ShaderProgram* overlayProgram;
            GLuint rectVAO;
    };
}
