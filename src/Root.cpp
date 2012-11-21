#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>

#include "../include/Root.h"
#include "../include/Shaders.h"

using std::cerr;
using std::cout;
using std::endl;

namespace Arya
{
    bool Root::init()
    {

        if(!initGLFW()) return false;
        if(!initGLEW()) return false;

        if(!initShaders()) return false;
        if(!initObjects()) return false;

        bool running = true;
        while(running)
        {
            render();
            running = (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));
        }

        glfwTerminate();

        return true;
    }

    bool Root::initGLFW()
    {
        if(!glfwInit())
        {
            cerr << "Could not init *glfw*" << endl;
            return false;
        }

        if(!glfwOpenWindow(1024, 768, 0, 0, 0, 0, 0, 0, GLFW_WINDOW))
        {
            glfwTerminate();
            return false;
        }

        return true;
    }

    bool Root::initGLEW()
    {
        glewInit();
        return true;
    }

    bool Root::initShaders()
    {
        shaders = new ShaderManager();
        if(!(shaders->init()))
            return false;
        return true;
    }

    bool Root::initObjects()
    {
        return true;
    }

    void Root::render()
    {
        glClearColor(0.0, 1.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers();
    }
}
