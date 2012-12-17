#include "../include/Server.h"
#include "../include/Packet.h"
#include "../include/EventCodes.h"
#include "../include/ServerClientHandler.h"
#include "../include/ServerClient.h"
#include "../include/Units.h"
#include "Arya.h"
#include <cstring>
#include <algorithm>

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
    clientIdFactory = 100;
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

Packet* Server::createPacket(int id)
{
    return new Packet(id);
}

void Server::sendToAllClients(Packet* pak)
{
    for(clientIterator iter = clientList.begin(); iter != clientList.end(); ++iter)
    {
        iter->second->handler->sendPacket(pak);
    }
}

//------------------------------
// SERVER LOGIC
//------------------------------

void Server::newClient(ServerClientHandler* client)
{
    ServerClient* cl = new ServerClient(this, client);
    clientList.insert(std::make_pair(client, cl));
}

void Server::removeClient(ServerClientHandler* client)
{
    clientIterator cl = clientList.find(client);
    if(cl != clientList.end() )
    {
        //TODO:
        //Lots of stuff like sending a message
        //to everyone about the client being disconnected
        //Distribute their resources accross allies
        //and so on
        //This could be done in the deconstructor
        //if wanted
        delete cl->second;
        clientList.erase(cl);
    }
}

void Server::handlePacket(ServerClientHandler* clienthandler, Packet& packet)
{
    clientIterator iter = clientList.find(clienthandler);
    if(iter == clientList.end())
    {
        LOG_WARNING("Received packet from unkown client!");
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

            //Create faction
            client->createFaction();
            client->createStartUnits();

            int joinedCount = 0;
            for(clientIterator iter = clientList.begin(); iter != clientList.end(); ++iter)
            {
                if( iter->second->getClientId() != -1 ) ++joinedCount;
            }

            LOG_INFO("Clients joined: " << joinedCount);
            if(joinedCount >= 1)
            {
                pak = createPacket(EVENT_GAME_READY);
                //Send to all clients
                sendToAllClients(pak);

                pak = createPacket(EVENT_GAME_FULLSTATE);

                *pak << joinedCount; //player count
                //for each player:
                for(clientIterator iter = clientList.begin(); iter != clientList.end(); ++iter)
                {
                    if(iter->second->getClientId() == -1) continue;

                    *pak << iter->second->getClientId();

                    Faction* faction = iter->second->getFaction();
                    faction->serialize(*pak);

                    int unitCount = (int)faction->getUnits().size();

                    *pak << unitCount;
                    for(std::list<Unit*>::iterator iter = faction->getUnits().begin(); iter != faction->getUnits().end(); ++iter)
                    {
                        (*iter)->serialize(*pak);
                    }
                }

                sendToAllClients(pak);
            }
            break;
        }
    default:
        LOG_INFO("Unknown package received..");
        break;
    }

    return;
}
