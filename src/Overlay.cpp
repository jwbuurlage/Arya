#include "Overlay.h"
#include "Root.h"
#include "Shaders.h"
#include "common/Logger.h"

namespace Arya
{
    Overlay::Overlay() 
    {
        rectVAO = 0;
        overlayProgram = 0;
    }

    Overlay::~Overlay()
    {

    }

    bool Overlay::init()
    {
        if(!initShaders()) return false;

        ww = Root::shared().getWindowWidth();
        wh = Root::shared().getWindowHeight();

        // make 1 by 1 px vertex buffer
        // inititialize shader
        GLfloat vertices[] = {
            0.0,        0.0,
            1.0 / ww,   0.0,
            0.0,        1.0 / wh,
            1.0 / ww,   1.0 / wh
        };

        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW); 

        glGenVertexArrays(1, &rectVAO);
        glBindVertexArray(rectVAO);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

        glBindVertexArray(0);

        return true;
    }

    bool Overlay::initShaders()
    {

        Shader* overlayVertex = new Shader(VERTEX);
        if(!(overlayVertex->addSourceFile("../shaders/overlay.vert"))) return false;
        if(!(overlayVertex->compile())) return false;

        Shader* overlayFragment = new Shader(FRAGMENT);
        if(!(overlayFragment->addSourceFile("../shaders/overlay.frag"))) return false;
        if(!(overlayFragment->compile())) return false;

        overlayProgram = new ShaderProgram("overlay");
        overlayProgram->attach(overlayVertex);
        overlayProgram->attach(overlayFragment);
        if(!(overlayProgram->link())) return false;

        return true;
    }

    void Overlay::addRect(Rect* rect)
    {
        rects.push_back(rect);
    }

    void Overlay::removeRect(Rect* rect)
    {
        for(vector<Rect*>::iterator it = rects.begin(); it != rects.end();)
        {
            if( *it == rect ) it = rects.erase(it);
            else ++it;
        }
    }

    void Overlay::render()
    {
        glDisable(GL_DEPTH_TEST);

        // bind shader
        overlayProgram->use();

        glBindVertexArray(rectVAO);

        // render all rects
        for(int i = 0; i < rects.size(); ++i) {
            // TODO: use dirty flag maybe?
            rects[i]->screenPosition = rects[i]->relative + vec2(2.0 * rects[i]->pixelOffset.x / ww, 2.0 * rects[i]->pixelOffset.y / wh);

            overlayProgram->setUniform2fv("pixelSize", rects[i]->pixelSize);
            overlayProgram->setUniform2fv("screenPosition", rects[i]->screenPosition);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        glEnable(GL_DEPTH_TEST);
    }
}
