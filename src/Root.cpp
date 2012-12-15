#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>

#include "Root.h"
#include "Models.h"
#include "Shaders.h"
#include "Textures.h"
#include "Scene.h"
#include "Files.h"
#include "Overlay.h"
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
        scene = 0;
        oldTime = 0.0;
        overlay = 0;

        Logger* log = new Logger();
        FileSystem* files = new FileSystem();
        TextureManager* tex = new TextureManager();
        ModelManager* modelManager = new ModelManager();
    }

    Root::~Root()
    {
        //TODO: Check if GLEW, GLFW, Shaders, Objects were still initated
        //Only clean them up if needed
        glfwTerminate();

        if(scene) delete scene;
        if(overlay) delete overlay;

        delete &ModelManager::shared();
        delete &FileSystem::shared();
        delete &Logger::shared();
        delete &TextureManager::shared();
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
        ModelManager::shared().initialize();

        overlay = new Overlay();
        if(!overlay->init()) return false;

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
        running = true;
        while(running)
        {
            if(!oldTime)
                oldTime = glfwGetTime();
            else {
                double pollTime = glfwGetTime();
                double elapsed = pollTime - oldTime;

                for(std::vector<FrameListener*>::iterator it = frameListeners.begin(); it != frameListeners.end();++it)
                    (*it)->onFrame((float)elapsed);

                oldTime = pollTime;
            }

            render();

            glfwPollEvents();
            if( glfwGetWindowParam(GLFW_OPENED) == 0 ) running = false;
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

        glfwCloseWindow();

        if(!glfwOpenWindow(windowWidth, windowHeight, 0, 0, 0, 0, 32, 0, (fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW)))
        {
            LOG_ERROR("Could not re-create window. Closing now.");
            stopRendering();
            return;
        }

        glfwSetWindowTitle("Arya");
        glfwEnable(GLFW_MOUSE_CURSOR);
        glfwSetKeyCallback(keyCallback);
        glfwSetMouseButtonCallback(mouseButtonCallback);
        glfwSetMousePosCallback(mousePosCallback);
        glfwSetMouseWheelCallback(mouseWheelCallback);
    }

    bool Root::initGLFW()
    {
        if(!glfwInit())
        {
            cerr << "Could not init *glfw*" << endl;
            return false;
        }

        GLFWvidmode mode;
        glfwGetDesktopMode(&mode);
        desktopWidth = mode.Width;
        desktopHeight = mode.Height;
        if(fullscreen)
        {
            windowWidth = desktopWidth;
            windowHeight = desktopHeight;
        }

#ifdef __APPLE__
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3); // Use OpenGL Core v3.2
        glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
#endif

        if(!glfwOpenWindow(windowWidth, windowHeight, 0, 0, 0, 0, 32, 0, (fullscreen ? GLFW_FULLSCREEN : GLFW_WINDOW)))
        {
            return false;
        }

        glfwSetWindowTitle("Arya");
        glfwEnable(GLFW_MOUSE_CURSOR);
        glfwSetKeyCallback(keyCallback);
        glfwSetMouseButtonCallback(mouseButtonCallback);
        glfwSetMousePosCallback(mousePosCallback);
        glfwSetMouseWheelCallback(mouseWheelCallback);

        return true;
    }

    bool Root::initGLEW()
    {
#ifdef __APPLE__
        glewExperimental = GL_TRUE; 
#endif
        glewInit();

        if (!GLEW_VERSION_4_0)
        {
            LOG_WARNING("No OpenGL 4.0 support! Continuing");
        }

        return true;
    }

    void Root::render()
    {
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        if(scene)
            scene->render();

        // read from depth buffer
        GLfloat depth;
        glReadPixels(200, 200, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &depth);
        // and do nothing with it, yay :)

        if(overlay)
            overlay->render();

        for(std::vector<FrameListener*>::iterator it = frameListeners.begin(); it != frameListeners.end();++it)
            (*it)->onRender();

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
            if( (*it)->mouseDown((MOUSEBUTTON)button, action == GLFW_PRESS, mouseX, windowHeight - mouseY) == true ) break;
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
            if( (*it)->mouseMoved(x, windowHeight - y, dx, dy) == true ) break;
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
