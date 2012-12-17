#include "../include/Server.h"
#include "../include/Packet.h"
#include "../include/EventCodes.h"
#include "../include/ServerClientHandler.h"
#include "Arya.h"
#include <cstring>
#include <iostream>

#include "Poco/Exception.h"
#include "Poco/Net/NetException.h"
#include "Poco/NObserver.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"

using namespace Poco;
using namespace Poco::Net;

Server::Server()
{
    serverSocket = 0;
    reactor = 0;
    acceptor = 0;
    port = 1337;
}

Server::~Server()
{
    if(thread.isRunning())
    {
        LOG_INFO("Waiting for server to finish");
        reactor->stop();
        thread.join();
    }
    if(acceptor) delete acceptor;
    if(reactor) delete reactor;
    if(serverSocket) delete serverSocket;
}

void Server::runInThread()
{
    if(acceptor) delete acceptor;
    if(reactor) delete reactor;
    if(serverSocket) delete serverSocket;

    //Multiple acceptors can register to the reactor: one for every port for example
    //Only a single reactor can run at a single moment
    reactor = new SocketReactor;

    //Create the server socket
    IPAddress any_address;
    SocketAddress serveraddr(any_address, port);
    serverSocket = new ServerSocket(serveraddr);

    //Create the acceptor that will listen on the server socket
    //It will register to the reactor
    acceptor = new ConnectionAcceptor(*serverSocket, *reactor, this);

    thread.start(*reactor);
    LOG_INFO("Server started on port " << port);
}

//------------------------------
// SERVER LOGIC
//------------------------------

void Server::handlePacket(Packet& packet)
{
    int id = packet.getId();
    switch(packet.getId()) {
    case EVENT_JOIN_GAME:
        LOG_INFO("someone joined... what do");
        break;

    default:
        LOG_INFO("Unknown package received..");
        break;
    }

    return;
}
