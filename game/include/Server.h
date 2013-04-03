#pragma once
#include <map>
#include <vector>

#include "SFML/System.hpp"
#include "Poco/Thread.h"
#include "Poco/Net/ServerSocket.h"

using std::vector;
using std::map;

class ServerClientHandler;
class ConnectionAcceptor;
class ServerReactor;
class Packet;
class Server;
class ServerClient;
class ServerGameSession;
class Scripting;

class Server
{
    public:
        Server();
        ~Server();

        void run();
        void runInThread();

        void update(); //called periodically by reactor
        void newClient(ServerClientHandler* client);
        void removeClient(ServerClientHandler* client);
        void handlePacket(ServerClientHandler* client, Packet& packet);

        //When creating a packet
        //it MUST be added to at least
        //one ServerClientHandler or
        //else it will be a memory leak
        //TODO: also save Packet* list
        //in Server, and on each update
        //check for zero refcounts
        Packet* createPacket(int id);

        void deletePacket(Packet* pak);

        Scripting* getScripting() const { return scripting; }

    private:
        Poco::Thread thread;
        Poco::Net::ServerSocket* serverSocket;
        ServerReactor* reactor;
        ConnectionAcceptor* acceptor;
        int port;
        void prepareServer();

        sf::Clock timer;
        sf::Time timerDiff;
        unsigned int frameCounter;
        sf::Time fpsTime;

        Scripting* scripting;

        //List of all clients, can be in any game session
        //ServerClient contains a pointer to their game session
        map<ServerClientHandler*,ServerClient*> clientList;
        typedef map<ServerClientHandler*,ServerClient*>::iterator clientIterator;
        int clientIdFactory;

        //ServerGameSession contains a list of ServerClients
        map<int,ServerGameSession*> sessionList;
        typedef map<int,ServerGameSession*>::iterator sessionIterator;
        int sessionIdFactory;
};
