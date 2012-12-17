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
            Overlay();
            ~Overlay();

            bool init();
            bool initShaders();

            void addRect(Rect* quad);
            void removeRect(Rect* quad);

            void render();

        private:
            vector<Rect*> rects;
            ShaderProgram* overlayProgram;
            GLuint rectVAO;

            int ww; //window width
            int wh; //window height
    };
}
