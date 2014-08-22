#include "Game.h"

int main()
{
    Game* game = new Game();

    if(!game->init()) {
        return -1;
    }

    game->run();

    return 0;
}
