#include "Root.h"
#include "World.h"
#include "Interface.h"
#include "Graphics.h"
#include "InputSystem.h"
#include "common/Logger.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

namespace Arya
{
    //Little container that holds the SDL variables
    struct SDLValues
    {
        SDL_Window* window;
        SDL_GLContext context;

        SDLValues() : window(0), context(0) {}
        ~SDLValues(){}
    };

    Root::Root()
    {
        sdlValues = new SDLValues;

        world = new World;
        interface = new Interface;
        graphics = new Graphics;
        inputSystem = new InputSystem;

        loopRunning = false;
        windowWidth = 0;
        windowHeight = 0;
        fullscreen = false;
    }

    Root::~Root()
    {
        delete inputSystem;
        delete graphics;
        delete interface;
        delete world;

        if( sdlValues->context ) SDL_GL_DeleteContext(sdlValues->context);
        if( sdlValues->window ) SDL_DestroyWindow(sdlValues->window);
        delete sdlValues;
        SDL_Quit();
    }

    bool Root::init(const char* windowTitle, int _width, int _height, bool _fullscreen)
    {
        if( SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS) != 0 ) {
            LogError << "Failed to initialize SDL. Error message: " << SDL_GetError() << endLog;
            return false;
        }

        fullscreen = _fullscreen;
        if(fullscreen){
            //TODO: Multiscreen
            //SDL_GetNumVideoDisplays --> Decide which display to use
            SDL_DisplayMode mode;
            SDL_GetDesktopDisplayMode(0, &mode);
            windowWidth = mode.w;
            windowHeight = mode.h;
        }else{
            windowWidth = _width;
            windowHeight = _height;
        }

        //Request OpenGL 3.2 Core
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        //Additional OpenGL settings
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

        sdlValues->window = SDL_CreateWindow(windowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                windowWidth, windowHeight, (fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0) | SDL_WINDOW_OPENGL);

        if( !sdlValues->window ) {
            LogError << "Failed to create window with SDL. Error message: " << SDL_GetError() << endLog;
            return false;
        }

        sdlValues->context = SDL_GL_CreateContext(sdlValues->window);
        if( !sdlValues->context ) {
            LogError << "Failed to create OpenGL context. Error message: " << SDL_GetError() << endLog;
            return false;
        }

        //if( !graphics->init() ) return false;
        //if( !inputSystem->init() ) return false;
        //if( !interface->init() ) return false;
        //if( !world->init() ) return false;

        return true;
    }

    void Root::gameLoop( std::function<void(float)> callback )
    {
        LogInfo << "Game loop started." << endLog;
        while(loopRunning) {
            //Calling OpenGL draw functions will queueu instructions for the GPU
            //When calling SwapBuffers the program waits untill the GPU is done
            //Therefore that is the moment that we should do the game logic and physics
            //and so on and do the buffer swap afterwards.
            render();

//			if(!oldTime)
//				oldTime = glfwGetTime();
//			else {
//				double pollTime = glfwGetTime();
//				double elapsed = pollTime - oldTime;
//
//				//Note: it can happen that the list is modified
//				//during a call to onFrame()
//				//Some framelisteners (network update) add other framelisteners
//				for(std::list<FrameListener*>::iterator it = frameListeners.begin(); it != frameListeners.end();++it)
//					(*it)->onFrame((float)elapsed);
//
                float elapsed = 10.0f;
                callback(elapsed);
//
//				oldTime = pollTime;
//			}

            //Handle input
            SDL_Event event;
            while(SDL_PollEvent(&event)) {
                switch(event.type) {
                    case SDL_KEYDOWN:
                        break;
                    case SDL_KEYUP:
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                    case SDL_MOUSEBUTTONUP:
                    case SDL_MOUSEMOTION:
                        break;
                    default:
                        LogDebug << "Unkown SDL event: " << event.type << endLog;
                        break;
                }
            }
            //Swap buffers
            SDL_GL_SwapWindow(sdlValues->window);
        }
    }

    void Root::stopGameLoop()
    {
        loopRunning = false;
    }

    void Root::setFullscreen(bool fullscreen)
    {
        //TODO
        if( fullscreen ){

        } else {

        }
        return;
    }

    void Root::render()
    {

    }

    bool Root::initGLEW()
    {
        glewExperimental = GL_TRUE; 
        glewInit();

        if (!GLEW_VERSION_3_1)
            LogWarning << "No OpenGL 3.1 support! Continuing" << endLog;

        return true;
    }

}
