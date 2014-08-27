#include "common/Logger.h"
#include "Camera.h"
#include "Graphics.h"
#include "Renderer.h"

#include <GL/glew.h>

namespace Arya
{
    Graphics::Graphics()
    {
        renderer = new Renderer;
        camera = new Camera;
        materialSystem = 0;
        textureSystem = 0;
    }

    Graphics::~Graphics()
    {
        delete camera;
        delete renderer;
    }

    bool Graphics::init()
    {
        glewExperimental = GL_TRUE; 
        glewInit();
        if (!GLEW_VERSION_3_1)
            LogWarning << "No OpenGL 3.1 support! Continuing" << endLog;
        return true;
    }

    void Graphics::clear(int width, int height)
    {
        //glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
    }

    void Graphics::render()
    {
    }

}
