#include "Game.h"

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
}

void Game::run()
{
    // enter game loop
    // root returns false if engine closed
    root->gameLoop( [] (float dt) { dt += 0.1; } );
}
