#include "Root.h"
#include "World.h"
#include "Interface.h"
#include "Graphics.h"
#include "InputSystem.h"

namespace Arya
{
    Root::Root()
    {
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
    }

    bool Root::init()
    {
        return false;
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
}
