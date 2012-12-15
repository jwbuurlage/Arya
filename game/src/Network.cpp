#include "../include/Network.h"
#include "../include/Server.h"

Network::Network()
{
    server = 0;
}

Network::~Network()
{
    if(server) delete server;
}

void Network::startServer()
{
    if(server) delete server;
    server = new Server;
    server->runInThread();
}
