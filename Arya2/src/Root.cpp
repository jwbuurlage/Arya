#include "common/Logger.h"
#include "Files.h"
#include "Graphics.h"
#include "InputSystem.h"
#include "Interface.h"
#include "Locator.h"
#include "Root.h"
#include "World.h"

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

        fileSystem = new FileSystem();
        Locator::provide(fileSystem);

        world = new World;
        interface = new Interface;
        graphics = new Graphics;
        inputSystem = new InputSystem;

        loopRunning = false;
        windowWidth = 0;
        windowHeight = 0;
        fullscreen = false;
        timer = 0;
    }

    Root::~Root()
    {
        delete inputSystem;
        delete graphics;
        delete interface;
        delete world;

        delete fileSystem;
        fileSystem = 0;
        Locator::provide(fileSystem);

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

        if( !graphics->init() ) return false;
        //if( !inputSystem->init() ) return false;
        //if( !interface->init() ) return false;
        //if( !world->init() ) return false;

        return true;
    }

    void Root::gameLoop( std::function<void(float)> callback )
    {
        LogInfo << "Game loop started." << endLog;
        loopRunning = true;
        timer = SDL_GetTicks();
        while(loopRunning) {
            //Calling OpenGL draw functions will queueu instructions for the GPU
            //When calling SwapBuffers the program waits untill the GPU is done
            //Therefore that is the moment that we should do the game logic and physics
            //and so on and do the buffer swap afterwards.
            render();

            int pollTime = SDL_GetTicks();
            float elapsed = pollTime - timer;
            timer = pollTime;

            callback(elapsed);
            graphics->update(elapsed);

            handleEvents();

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

    void Root::handleEvents()
    {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_KEYDOWN:
                case SDL_KEYUP:
                case SDL_MOUSEBUTTONDOWN:
                case SDL_MOUSEBUTTONUP:
                case SDL_MOUSEMOTION:
                case SDL_MOUSEWHEEL:
                    inputSystem->handleInputEvent(event);
                    break;
                case SDL_TEXTINPUT:
                    //TODO
                    //If the key pressed is a letter or any other
                    //text based input, SDL gives this event
                    //which can be used for properly using textboxes
                    //so that keys like ^ ' ~ work as expected
                    break;
                case SDL_QUIT:
                    stopGameLoop();
                    break;
                case SDL_WINDOWEVENT:
                    switch(event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED:
                            windowResized(event.window.data1, event.window.data2);
                            break;
                        case SDL_WINDOWEVENT_CLOSE:
                            stopGameLoop();
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    LogDebug << "Unkown SDL event: " << event.type << endLog;
                    break;
            }
        }

    }

    void Root::render()
    {
        graphics->clear(getWindowWidth(), getWindowHeight());
    }

    void Root::windowResized(int newWidth, int newHeight)
    {
		windowWidth = newWidth;
		windowHeight = newHeight;
        //TODO:
        //graphics->getCamera()
        //cam->setProjectionMatrix(45.0f windowWidth/((float)windowHeight), 0.1f, 2000.0f);
        //
        //interface->recalculatePositions()
    }
}
