#include "../include/Game.h"
#include "../include/Network.h"
#include <cstring>

int main(int argc, char* argv[])
{
    bool serverOnly = false;

    if(argc >= 2)
    {
        if(!strcmp(argv[1],"--server"))
        {
            serverOnly = true;
        }
    }

    if(serverOnly)
    {
        Network* network = new Network;
        network->runServer();
    }
    else
    {
        Game* game = new Game;
        game->run();
        delete game;
    }
    return 0;
}
