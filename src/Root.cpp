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
using std::endl;


namespace Arya
{

	template<> Root* Singleton<Root>::singleton = 0;

    //glfw callback functions
    void GLFWCALL keyCallback(int key, int action);
    void GLFWCALL mouseButtonCallback(int button, int action);
    void GLFWCALL mousePosCallback(int x, int y);
    void GLFWCALL mouseWheelCallback(int pos);

    Root::Root()
    {
        Logger* log = new Logger();
        FileSystem* files = new FileSystem();
    }

    Root::~Root()
    {
        //TODO: Check if GLEW, GLFW, Shaders, Objects were still initated
        //Only clean them up if needed
        glfwTerminate();

        delete &FileSystem::shared();
        delete &Logger::shared();
    }

    bool Root::init()
    {
        if(!initGLFW()) return false;
        if(!initGLEW()) return false;

        if(!initShaders()) return false;
        if(!initObjects()) return false;

        scene = new Scene();

        return true;
    }

    void Root::startRendering()
    {
        running = true;
        while(running)
        {
            render();
            glfwPollEvents();
            if( glfwGetWindowParam(GLFW_OPENED) == 0 ) running = false;
        }
    }

    void Root::stopRendering()
    {
        running = false;
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

        glfwSetKeyCallback(keyCallback);
        glfwSetMouseButtonCallback(mouseButtonCallback);
        glfwSetMousePosCallback(mousePosCallback);
        glfwSetMouseWheelCallback(mouseWheelCallback);

        return true;
    }

    bool Root::initGLEW()
    {
        glewInit();
        return true;
    }

    bool Root::initShaders()
    {
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

        return true;
    }

    bool Root::initObjects()
    {
        return true;
    }

    void Root::render()
    {
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        scene->render();

        glfwSwapBuffers();
    }

	void Root::addInputListener(InputListener* listener)
	{
		inputListeners.push_back(listener);
	}

	void Root::removeInputListener(InputListener* listener)
	{
		for( std::vector<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ){
			if( *it == listener ) it = inputListeners.erase(it);
			else ++it;
		}
	}

	void Root::addFrameListener(FrameListener* listener)
	{
		frameListeners.push_back(listener);
	}

	void Root::removeFrameListener(FrameListener* listener)
	{
		for( std::vector<FrameListener*>::iterator it = frameListeners.begin(); it != frameListeners.end(); ){
			if( *it == listener ) it = frameListeners.erase(it);
			else ++it;
		}
	}

	void Root::keyDown(int key, int action)
    {
		for( std::vector<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ++it )
			if( (*it)->keyDown(key, action == GLFW_PRESS) == true ) break;

	}

	void Root::mouseDown(int button, int action)
    {
		for( std::vector<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ++it )
			if( (*it)->mouseDown((MOUSEBUTTON)button, action == GLFW_PRESS, mouseX, mouseY) == true ) break;
	}

	void Root::mouseWheelMoved(int pos)
    {
        int delta = pos - mouseWheelPos;
        mouseWheelPos = pos;
		for( std::vector<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ++it )
			if( (*it)->mouseWheelMoved(delta) == true ) break;
	}

	void Root::mouseMoved(int x, int y)
    {
		int dx = x - mouseX, dy = y - mouseY;
		mouseX = x; mouseY = y;
		for( std::vector<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ++it )
			if( (*it)->mouseMoved(x,y, dx, dy) == true ) break;
	}

    void GLFWCALL keyCallback(int key, int action)
    {
        Root::shared().keyDown(key, action);
    }

    void GLFWCALL mouseButtonCallback(int button, int action)
    {
        Root::shared().mouseDown(button, action);
    }

    void GLFWCALL mousePosCallback(int x, int y)
    {
        Root::shared().mouseMoved(x, y);
    }

    void GLFWCALL mouseWheelCallback(int pos)
    {
        Root::shared().mouseWheelMoved(pos);
    }
}
