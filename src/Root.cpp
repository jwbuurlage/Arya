#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Root.h"
#include "Models.h"
#include "Shaders.h"
#include "Fonts.h"
#include "Textures.h"
#include "Scene.h"
#include "Decals.h"
#include "Console.h"
#include "Config.h"
#include "Commands.h"
#include "Files.h"
#include "Overlay.h"
#include "Camera.h"
#include "Sounds.h"
#include "common/Logger.h"
#include "Interface.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace Arya
{
    template<> Root* Singleton<Root>::singleton = 0;

    //glfw callback functions
    void mousePosCallback   (GLFWwindow* win, double x, double y);
    void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods);
    void keyCallback        (GLFWwindow* win, int key, int scancode, int action, int mods);
    void mouseWheelCallback (GLFWwindow* win, double scrollX, double scrollY);
    void windowSizeCallback (GLFWwindow* win, int width, int height);

    Root::Root()
    {
        scene = 0;
        oldTime = 0;
        interface = 0;
        glfwWindow = 0;
        readDepthNextFrame = false;

        FileSystem::create();
        CommandHandler::create();
        Config::create();
        TextureManager::create();
        MaterialManager::create();
        ModelManager::create();
        FontManager::create();
        SoundManager::create();
        Console::create();
        Decals::create();

        //Some classes should be initialized
        //before the graphics, like Config.
        //Other graphic related classes are
        //initialized in Root::initialize
        //when the graphics are initialized
        if(!Config::shared().init()) LOG_WARNING("Unable to init config");
    }

    Root::~Root()
    {
        //Console deconstructor uses overlay
        //so it must be deleted first
        Console::destroy();

        if(scene) delete scene;
        if(interface) delete interface;

        SoundManager::destroy();
        FontManager::destroy();
        ModelManager::destroy();
        MaterialManager::destroy();
        TextureManager::destroy();
        Config::destroy();
        CommandHandler::destroy();
        FileSystem::destroy();
		Decals::destroy();

        //TODO: Check if GLEW, GLFW, Shaders, Objects were still initated
        //Only clean them up if needed
        glfwTerminate();
    }

    bool Root::init(bool fullscr, int w, int h)
    {
        LOG_INFO("loading root");

        windowWidth = w;
        windowHeight = h;
        fullscreen = fullscr;

        if(!initGLFW()) return false;
        if(!initGLEW()) return false;

        // set GL stuff
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //Call these in the right order: Models need Textures
        TextureManager::shared().initialize();
		//MaterialManager::shared().initialize();
		ModelManager::shared().initialize();
		if(!SoundManager::shared().init())
		{
			LOG_WARNING("Could not initialize SoundManager, files not found!");
		}

        if(!FontManager::shared().initialize())
        {
            LOG_ERROR("Could not initialize FontManager, files not found!");
            return false;
        }

		if(!interface) interface = new Interface;
		if(!interface->init())
		{
			LOG_ERROR("Could not initialize interface");
			return false;
		}
		addFrameListener(interface);

		if(!Console::shared().init()) 
		{
			LOG_ERROR("Could not initialize console");
			return false;
		}
		addFrameListener(&Console::shared());
		addInputListener(&Console::shared());

		Decals::shared().init();

		LOG_INFO("Root initialized");

		return true;
	}

	Scene* Root::makeDefaultScene()
	{
		if(!scene) delete scene;

		scene = new Scene;
		if( !scene->isInitialized() )
		{
			LOG_ERROR("Unable to initialize scene");
			delete scene;
			scene = 0;
			return 0;
		}
		else
			addFrameListener(scene);

		LOG_INFO("Made scene");

		return scene;
	}

	void Root::removeScene()
	{
		removeFrameListener(scene);
		if(scene) delete scene;
		scene = 0;
	}

	void Root::startRendering()
	{
		LOG_INFO("start rendering");
		running = true;
		while(running)
		{
            //Calling OpenGL draw functions will queueu instructions for the GPU
            //When calling glfwSwapBuffers the program waits untill the GPU is done
            //Therefore that is the moment that we should do the game logic and physics
            //and so on and do the buffer swap afterwards.
            render();

			if(!oldTime)
				oldTime = glfwGetTime();
			else {
				double pollTime = glfwGetTime();
				double elapsed = pollTime - oldTime;

				//Note: it can happen that the list is modified
				//during a call to onFrame()
				//Some framelisteners (network update) add other framelisteners
				for(std::list<FrameListener*>::iterator it = frameListeners.begin(); it != frameListeners.end();++it)
					(*it)->onFrame((float)elapsed);

				oldTime = pollTime;
			}

            //Handle input
            glfwPollEvents();
            if( glfwWindowShouldClose(glfwWindow) ) running = false;

            if(readDepthNextFrame && scene)
            {
                GLfloat depth;
                glReadPixels(mouseX, mouseY, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);

                vec4 screenPos(2.0f * mouseX /((float)windowWidth) - 1.0f, 2.0f * mouseY/((float)windowHeight) - 1.0f, 2.0f*depth-1.0f, 1.0);

                screenPos = scene->getCamera()->getInverseVPMatrix() * screenPos;
                screenPos /= screenPos.w; 

                clickScreenLocation.x = screenPos.x;
                clickScreenLocation.y = screenPos.y;
                clickScreenLocation.z = screenPos.z;

                readDepthNextFrame = false;
            }
            //Swap buffers
            glfwSwapBuffers(glfwWindow);
		}
	}

	void Root::stopRendering()
	{
		running = false;
	}

	void Root::setFullscreen(bool fullscr)
	{
		if( fullscreen == fullscr ) return; //no difference
		fullscreen = fullscr;

        if(glfwWindow)
            glfwDestroyWindow(glfwWindow);

		//if(!glfwOpenWindow(windowWidth, windowHeight, 0, 0, 0, 0, 32, 0, (fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW)))
        glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "Arya", NULL, NULL);
        if(!glfwWindow)
		{
			LOG_ERROR("Could not re-create window. Closing now.");
			stopRendering();
			return;
		}

        glfwMakeContextCurrent(glfwWindow);
        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetWindowSizeCallback(glfwWindow, windowSizeCallback);
		glfwSetKeyCallback(glfwWindow, keyCallback);
		glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
		glfwSetCursorPosCallback(glfwWindow, mousePosCallback);
		glfwSetScrollCallback(glfwWindow, mouseWheelCallback);
	}

	bool Root::initGLFW()
	{
		if(!glfwInit())
		{
			LOG_ERROR("Could not init glfw!");
			return false;
		}

		const GLFWvidmode* mode = glfwGetVideoMode( glfwGetPrimaryMonitor() );
		desktopWidth = mode->width;
		desktopHeight = mode->height;
		if(fullscreen)
		{
			windowWidth = desktopWidth;
			windowHeight = desktopHeight;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // Use OpenGL Core v3.2
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

		//if(!glfwOpenWindow(windowWidth, windowHeight, 0, 0, 0, 0, 32, 0, (fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW)))
        glfwWindow = glfwCreateWindow(windowWidth, windowHeight, "Arya", NULL, NULL);
        if(!glfwWindow)
		{
			LOG_ERROR("Could not open glfw window!");
			return false;
		}

        //Every window has an OpenGL context. Make this one active.
        glfwMakeContextCurrent(glfwWindow);

        glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		glfwSetWindowSizeCallback(glfwWindow, windowSizeCallback);
		glfwSetKeyCallback(glfwWindow, keyCallback);
		glfwSetMouseButtonCallback(glfwWindow, mouseButtonCallback);
		glfwSetCursorPosCallback(glfwWindow, mousePosCallback);
		glfwSetScrollCallback(glfwWindow, mouseWheelCallback);

		return true;
	}

	bool Root::initGLEW()
	{
		glewExperimental = GL_TRUE; 
		glewInit();

		if (!GLEW_VERSION_3_1)
		{
			LOG_WARNING("No OpenGL 3.1 support! Continuing");
		}

		return true;
	}

	void Root::render()
	{
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, getWindowWidth(), getWindowHeight());
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		if(scene)
		{
			scene->render();

			for(std::list<FrameListener*>::iterator it = frameListeners.begin(); it != frameListeners.end();)
			{
				//This construction allows the callback to erase itself from the frameListeners list
				std::list<FrameListener*>::iterator iter = it++;
				(*iter)->onRender();
			}

		}
		if(interface)
			interface->render();
	}

	Overlay* Root::getOverlay() const
	{
		return interface->getOverlay();
	}

	mat4 Root::getPixelToScreenTransform() const
	{
		return glm::scale(mat4(1.0), vec3(2.0/windowWidth, 2.0/windowHeight, 1.0));
	}

	void Root::addInputListener(InputListener* listener)
	{
		inputListeners.push_back(listener);
	}

	void Root::removeInputListener(InputListener* listener)
	{
		for( std::list<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ){
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
		for( std::list<FrameListener*>::iterator it = frameListeners.begin(); it != frameListeners.end(); ){
			if( *it == listener ) it = frameListeners.erase(it);
			else ++it;
		}
	}

	void Root::windowSizeChanged(int width, int height)
	{
		windowWidth = width;
		windowHeight = height;
		if(scene)
		{
			Camera* cam = scene->getCamera();
			if(cam)
				cam->setProjectionMatrix(45.0f, getAspectRatio(), 0.1f, 2000.0f);
		}
        if(interface) interface->recalculatePositions();
	}

	void Root::keyDown(int key, int action)
	{
		for( std::list<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ++it )
			if( (*it)->keyDown(key, action == GLFW_PRESS) == true ) break;

	}

	void Root::mouseDown(int button, int action)
	{
		for( std::list<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ++it )
			if( (*it)->mouseDown((MOUSEBUTTON)button, action == GLFW_PRESS, mouseX, mouseY) == true ) break;
	}

	void Root::mouseWheelMoved(int pos)
	{
		int delta = pos - mouseWheelPos;
		mouseWheelPos = pos;
		for( std::list<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ++it )
			if( (*it)->mouseWheelMoved(delta) == true ) break;
	}

	void Root::mouseMoved(int x, int y)
	{
		y = windowHeight - y;
		int dx = x - mouseX, dy = y - mouseY;
		mouseX = x; mouseY = y;

		for( std::list<InputListener*>::iterator it = inputListeners.begin(); it != inputListeners.end(); ++it )
			if( (*it)->mouseMoved(x, y, dx, dy) == true ) break;
	}

	void windowSizeCallback(GLFWwindow* win, int width, int height)
	{
		Root::shared().windowSizeChanged(width, height);
	}

	void keyCallback(GLFWwindow* win, int key, int scancode, int action, int mods)
	{
		Root::shared().keyDown(key, action);
	}

	void mouseButtonCallback(GLFWwindow* win, int button, int action, int mods)
	{
		Root::shared().mouseDown(button, action);
	}

	void mousePosCallback(GLFWwindow* win, double x, double y)
	{
		Root::shared().mouseMoved(x, y);
	}

	void mouseWheelCallback(GLFWwindow* win, double scrollX, double scrollY)
	{
		Root::shared().mouseWheelMoved(scrollY);
	}
}
