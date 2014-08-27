#include "Game.h"
using namespace Arya;

Game::Game()
{

}

Game::~Game()
{

}

bool Game::init()
{
    root = new Arya::Root();

    if(!root->init("Minimal Example",
                1024, 768, false)) {
        return false;
    }

    root->getInputSystem()->bind(INPUT_KEYDOWN, [](int key){ LogInfo << "Game got keydown: " << key << endLog; });
    root->getInputSystem()->bind(INPUT_KEYUP, [](int key){ LogInfo << "Game got keyup: " << key << endLog; });

    return true;
}

void Game::run()
{
    // enter game loop
    // root returns false if engine closed
    root->gameLoop( [] (float dt) { dt += 0.1; } );
}
