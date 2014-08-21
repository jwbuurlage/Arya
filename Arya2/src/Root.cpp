#include "Root.h"
#include "World.h"
#include "Interface.h"
#include "Graphics.h"
#include "InputSystem.h"
#include "common/Logger.h"

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
            LogError << "Failed to initialize SDL. Error message: " << SDL_GetError() << endl;
            return false;
        }

        fullscreen = _fullscreen;
        if(fullscreen){
            //TODO: Multiscreen
            //SDL_GetNumVideoDisplays --> Decide which display to use
            SDL_DisplayMode mode;
            SDL_GetDesktopDisplayMode(0, &mode);
            windowWidth = mode.w;
            windowHeight = mode.h
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
            LogError << "Failed to create window with SDL. Error message: " << SDL_GetError() << endl;
            return false;
        }

        sdlValues->context = SDL_GL_CreateContext(window);
        if( !sdlValues->context ) {
            LogError << "Failed to create OpenGL context. Error message: " << SDL_GetError() << endl;
            return false;
        }

        if( !graphics->init() ) return false;
        //if( !inputSystem->init() ) return false;
        //if( !interface->init() ) return false;
        //if( !world->init() ) return false;

        return true;
    }

    void Root::startGameLoop()
    {
    }

    void Root::stopGameLoop()
    {
    }

    void Root::setFullscreen(bool fullscreen)
    {
        if( fullscreen ){

        } else {

        }
        return;
    }

    bool Root::initGLEW()
    {
        glewExperimental = GL_TRUE; 
        glewInit();

        if (!GLEW_VERSION_3_1)
            LogWarning << "No OpenGL 3.1 support! Continuing" << endl;

        return true;
    }

}
