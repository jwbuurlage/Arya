#include "../include/Game.h"

int main()
{
    Game* game = new Game;
    game->run();
    delete &Game::shared();
    return 0;
}
