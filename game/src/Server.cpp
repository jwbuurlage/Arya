#include "../include/common/GameLogger.h"
#include "../include/Server.h"
#include "../include/Packet.h"
#include "../include/EventCodes.h"
#include "../include/ServerClientHandler.h"
#include "../include/ServerGameSession.h"
#include "../include/ServerClient.h"
#include "../include/Units.h"
#include "../include/Scripting.h"
#include "Arya.h"
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
    scripting = 0;
    clientIdFactory = 100;
    sessionIdFactory = 10000;
}

Server::~Server()
{
    if(thread.isRunning())
    {
        GAME_LOG_INFO("Waiting for server to finish");
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

    if(scripting) delete scripting;
}

void Server::run()
{
    GameLoggerInstance->enableGameConsoleOutput(false);
    prepareServer();
    GAME_LOG_INFO("Server started on port " << port);
    reactor->run();
}

void Server::runInThread()
{
    GameLoggerInstance->enableGameConsoleOutput(false);
    prepareServer();
    thread.start(*reactor);
    GAME_LOG_INFO("Server started on port " << port);
}

void Server::prepareServer()
{
    if(acceptor) delete acceptor;
    if(reactor) delete reactor;
    if(serverSocket) delete serverSocket;

    //Multiple acceptors can register to the reactor: one for every port for example
    //Only a single reactor can run at a single moment
    reactor = new ServerReactor(this);

    //50 ms frametime
    reactor->setTimeout(50);

    //Create the server socket
    IPAddress any_address;
    SocketAddress serveraddr(any_address, port);
    serverSocket = new ServerSocket(serveraddr);

    //Create the acceptor that will listen on the server socket
    //It will register to the reactor
    acceptor = new ConnectionAcceptor(*serverSocket, *reactor, this);

    timer.restart();
    timerDiff = sf::Time::Zero;
    fpsTime = sf::Time::Zero;
    frameCounter = 0;

    //TODO: better solution
    //Arya::FileSystem should be made threadsafe?
    //By having two instances of FileSystem we would load many files
    //twice which would be stupid
    if(&Arya::FileSystem::shared() == 0) Arya::FileSystem::create();

    if(scripting == 0)
    {
        scripting = new Scripting;
        scripting->init();
        scripting->execute("units.lua");
        scripting->execute("maps.lua");
    }
}

Packet* Server::createPacket(int id)
{
    return new Packet(id);
}

void Server::deletePacket(Packet* pak)
{
    delete pak;
}

//------------------------------
// SERVER LOGIC
//------------------------------

void Server::update()
{
    //Display fps every minute
    frameCounter++;
    fpsTime += timer.getElapsedTime();
    if(fpsTime.asMilliseconds() >= 60000)
    {
        GAME_LOG_INFO("Server fps: " << ((float)frameCounter) / fpsTime.asSeconds() );
        fpsTime -= sf::milliseconds(60000);
        frameCounter = 0;
    }

    timerDiff += timer.restart();
    while(timerDiff.asMilliseconds() > 100)
    {
        timerDiff -= sf::milliseconds(100);
        for(sessionIterator iter = sessionList.begin(); iter != sessionList.end(); ++iter)
        {
            iter->second->update((float)(100.0f/1000.0f));
        }
    }
    //save some cpu time
    //Poco::Thread::yield();
    Poco::Thread::sleep(1);
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
        GAME_LOG_WARNING("Received packet (id = " << packet.getId() << ") from unkown client!");
        return;
    }
    ServerClient* client = iter->second;

    switch(packet.getId()){
		case EVENT_NEW_SESSION:
			//TODO: authenticate to see if this is an actual lobby server
			{
				int sessionHash;
				int clientCount;

				packet >> sessionHash;
				packet >> clientCount;

				GAME_LOG_INFO("THE SESSION MADE WAS: " << sessionHash);

				if(clientCount > 4)
				{
					GAME_LOG_WARNING("Lobby server tried to create session with " << clientCount << " players. This server can not handle this.");
					clientCount = 4;
				}

                ServerGameSession* session = 0;
				sessionIterator iter = sessionList.find(sessionHash);
				if(iter == sessionList.end())
				{
                    session = new ServerGameSession(this);
                    sessionList.insert(make_pair(sessionHash, session));

					session->getGameInfo().playerCount = clientCount;
					for(int i = 0; i < clientCount; ++i)
					{
						int hash;
						packet >> hash;
						session->getGameInfo().players[i].accountId = 0;
						session->getGameInfo().players[i].sessionHash = hash;
						session->getGameInfo().players[i].slot = i;
						session->getGameInfo().players[i].color = i;
						session->getGameInfo().players[i].team = 0;
					}
					session->initialize();
 				}
				else
				{
					GAME_LOG_WARNING("Lobby server tried to create sesison that already existed.");
				}
			}
			break;
        case EVENT_JOIN_GAME:
			if(client->getClientId() != -1)
			{
				GAME_LOG_WARNING("Client " << client->getClientId() << " requests to join when already joined.");
			}
			else
			{
				int sessionHash;
				int clientHash;
				packet >> sessionHash >> clientHash;

				GAME_LOG_INFO("SESSION HASH: " << sessionHash << " " << clientHash);

                ServerGameSession* session = 0;
				sessionIterator iter = sessionList.find(sessionHash);
				if(iter == sessionList.end())
				{
					GAME_LOG_INFO("Client requested to join non-existing session.");
				}
				else
				{
					if( iter->second->isGameStarted() )
						//TODO: send packet to client telling him to gtfo with his hacks or wooden pc
						GAME_LOG_WARNING("Client tried to join game that was already started. Refusing client.");
					else
						session = iter->second;
				}
				
				if(session)
				{
					//check if this client is allowed to join this session
					bool found = false;
					for(int i = 0; i < session->getGameInfo().playerCount; ++i)
					{
					    if(clientHash == session->getGameInfo().players[i].sessionHash)
						{
							//Accept the client: give him an id
							client->setClientId(clientIdFactory++);
							Packet* pak = createPacket(EVENT_CLIENT_ID);
							*pak << client->getClientId();
							clienthandler->sendPacket(pak);

							session->addClient(client, i);

							found = true;
							break;
						}
					}
					if(!found)
					{
						GAME_LOG_WARNING("Client tried to join game but had no valid client hash");
					}
				}
            }
            break;
        default:
            if(client->getClientId() == -1)
            {
                GAME_LOG_WARNING("Unkown packet (id = " << packet.getId() << ") received from client with no id");
            }
            else
            {
                if(client->getSession())
                    client->getSession()->handlePacket(client, packet);
                else
                    GAME_LOG_WARNING("Unkown packet (id = " << packet.getId() << ") received from client " << client->getClientId() << " with no session");
            }
            break;
    }
    return;
}
