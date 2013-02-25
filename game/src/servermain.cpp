#include "../include/common/GameLogger.h"
#include "../include/Network.h"

int main(int argc, char* argv[])
{
    GameLoggerInstance = new GameLogger;
	Network* network = new Network;
	network->runServer();
	delete GameLoggerInstance;
    return 0;
}
