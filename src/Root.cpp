#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include <glm/glm.hpp>

#include "../include/Root.h"
#include "../include/Shaders.h"
#include "Scene.h"
#include "Files.h"
#include "common/Logger.h"

using std::cerr;
using std::cout;
using std::endl;

namespace Arya
{
    bool Root::init()
    {
        Logger* dummyLogger = new Logger();
        FileSystem* dummyFileSystem = new FileSystem();

        if(!initGLFW()) return false;
        if(!initGLEW()) return false;

        if(!initShaders()) return false;
        if(!initObjects()) return false;

        scene = new Scene();

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
        cerr << "loading shaders" << endl;
        Shader* vertex = new Shader(VERTEX);
        if(!(vertex->addSourceFile("../shaders/basic.vert"))) return false;
        if(!(vertex->compile())) return false;

        Shader* fragment = new Shader(FRAGMENT);
        if(!(fragment->addSourceFile("../shaders/basic.frag"))) return false;
        if(!(fragment->compile())) return false;

        ShaderProgram* basicProgram = new ShaderProgram("basic");
        basicProgram->attach(vertex);
        basicProgram->attach(fragment);
        if(!(basicProgram->link())) return false;
        basicProgram->use();
        cerr << "finished loading shaders" << endl;

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

        scene->render();

        glfwSwapBuffers();
    }
}
