#include "../include/Game.h"
#include "../include/GameSession.h"

#include "common/Logger.h"

Game::Game()
{
    root = 0;
    goingForward = goingBackward = goingLeft = goingRight = goingUp = goingDown = false;
    mouseLeft = mouseRight = mouseTop = mouseBot = false;
    draggingLeftMouse = draggingRightMouse = false;
    forceDirection = vec3(0.0f);
    specMovement = vec3(0.0f);
    specPos = vec3(0.0f);
}

Game::~Game()
{
    if(root) delete &Root::shared();
}

void Game::run()
{
    root = new Root;

    if(!(root->init(true, 800, 600))) {
        LOG_ERROR("Unable to init root");
    }
    else
    {
        root->addInputListener(this);

        GameSession* session = new GameSession;
        session->init();
        session->start();

        root->startRendering();
    }
}

bool Game::keyDown(int key, bool keyDown)
{
    bool keyHandled = true;

    switch(key) {
        case 'S': keyHandled = false; break;
        case 'X': keyHandled = false; break;
        default: keyHandled = false; break;
    }

    return keyHandled;
}

bool Game::mouseDown(Arya::MOUSEBUTTON button, bool buttonDown, int x, int y)
{
    return false;
}

bool Game::mouseWheelMoved(int delta)
{
    return false;
}

bool Game::mouseMoved(int x, int y, int dx, int dy)
{
    return false; 
}

