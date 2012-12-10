#include "../include/Game.h"
#include "../include/GameSession.h"

#include "common/Logger.h"

Game::Game()
{
    root = 0;
    session = 0;
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

        root->startRendering();
    }
}

bool Game::keyDown(int key, bool keyDown)
{
    bool keyHandled = true;

    switch(key) {
        case 'P':
            if(keyDown) {
                if(session) delete session;
                session = new GameSession;
                session->init();
                session->start();
            }
            break;

        case 'L':
            if(keyDown)
                if(session) delete session;
            session = 0;
            break;

        case 'O': glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); break;
        case 'I': glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); break;
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

