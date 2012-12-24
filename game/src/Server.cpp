#include "../include/Server.h"
#include "../include/Packet.h"
#include "../include/EventCodes.h"
#include "../include/ServerClientHandler.h"
#include "../include/ServerGameSession.h"
#include "../include/ServerClient.h"
#include "../include/Units.h"
#include "Arya.h"
#include "../include/ServerLogger.h"
#include <cstring>
#include <algorithm>

#include "Poco/Exception.h"
#include "Poco/Net/NetException.h"
#include "Poco/NObserver.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"

using std::make_pair;
using namespace Poco;
using namespace Poco::Net;

Server::Server()
{
    serverSocket = 0;
    reactor = 0;
    acceptor = 0;
    port = 13337;
    clientIdFactory = 100;
    sessionIdFactory = 100;
    if(ServerLogger == 0) ServerLogger = new Arya::Logger;
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

    for(sessionIterator session = sessionList.begin(); session != sessionList.end(); ++session)
    {
        delete session->second;
    }
    sessionList.clear();
    for(clientIterator cl = clientList.begin(); cl != clientList.end(); ++cl)
    {
        delete cl->second;
    }
    clientList.clear();
}


void Server::run()
{
    prepareServer();
    LOG_INFO("Server started on port " << port);
    reactor->run();
}

void Server::runInThread()
{
    prepareServer();
    thread.start(*reactor);
    LOG_INFO("Server started on port " << port);
}

void Server::prepareServer()
{
    if(acceptor) delete acceptor;
    if(reactor) delete reactor;
    if(serverSocket) delete serverSocket;

    //Multiple acceptors can register to the reactor: one for every port for example
    //Only a single reactor can run at a single moment
    reactor = new ServerReactor(this);

    //20 ms frametime
    reactor->setTimeout(20);

    //Create the server socket
    IPAddress any_address;
    SocketAddress serveraddr(any_address, port);
    serverSocket = new ServerSocket(serveraddr);

    //Create the acceptor that will listen on the server socket
    //It will register to the reactor
    acceptor = new ConnectionAcceptor(*serverSocket, *reactor, this);

    //TODO: better solution
    //Arya::FileSystem should be made threadsafe?
    //By having two instances of FileSystem we would load many files
    //twice which would be stupid
    if(&Arya::FileSystem::shared() == 0) Arya::FileSystem::create();
}

Packet* Server::createPacket(int id)
{
    return new Packet(id);
}

//------------------------------
// SERVER LOGIC
//------------------------------

void Server::update()
{
    //diff is an 64 bit signed integer
    //with the elapsed time in microseconds
    Timestamp oldTime(timer);
    timer.update();
    Timestamp::TimeDiff diff = timer - oldTime;

    for(sessionIterator iter = sessionList.begin(); iter != sessionList.end(); ++iter)
    {
        iter->second->update((float)(diff/1000.0f));
    }
}

void Server::newClient(ServerClientHandler* clientHandler)
{
    ServerClient* cl = new ServerClient(this, clientHandler);
    clientList.insert(std::make_pair(clientHandler, cl));
}

void Server::removeClient(ServerClientHandler* clientHandler)
{
    clientIterator cl = clientList.find(clientHandler);
    if(cl != clientList.end() )
    {
        delete cl->second; //this will call GameSession::removeClient which will take care of ingame stuff
        clientList.erase(cl);
    }
}

void Server::handlePacket(ServerClientHandler* clienthandler, Packet& packet)
{
    clientIterator iter = clientList.find(clienthandler);
    if(iter == clientList.end())
    {
        LOG_WARNING("Received packet (id = " << packet.getId() << ") from unkown client!");
        return;
    }
    ServerClient* client = iter->second;

    switch(packet.getId()){
        case EVENT_JOIN_GAME:
            {
                client->setClientId(clientIdFactory++);

                Packet* pak = createPacket(EVENT_CLIENT_ID);
                *pak << client->getClientId();
                clienthandler->sendPacket(pak);

                //TODO: Check which session the client wants to join
                //For now: create a session if it doesnt exist
                //and else join the existing session
                ServerGameSession* session;
                if(sessionList.empty())
                {
                    session = new ServerGameSession(this);
                    sessionList.insert(make_pair(sessionIdFactory++, session));
                }
                else
                {
                    session = sessionList.begin()->second;
                }

                pak = createPacket(EVENT_GAME_READY);
                clienthandler->sendPacket(pak);

                session->addClient(client);
            }
            break;
        default:
            if(client->getClientId() == -1)
            {
                LOG_WARNING("Unkown packet (id = " << packet.getId() << ") received from client with no id");
            }
            else
            {
                if(client->getSession())
                    client->getSession()->handlePacket(client, packet);
                else
                    LOG_WARNING("Unkown packet (id = " << packet.getId() << ") received from client with no session");
            }
            break;
    }
    return;
}
