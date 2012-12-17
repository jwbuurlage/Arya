#pragma once
#include <map>
#include <vector>

#include "Poco/Thread.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketReactor.h"

using std::vector;
using std::map;

class ServerClientHandler;
class ConnectionAcceptor;
class Packet;
class Server;
class ServerClient;

class Server
{
    public:
        Server();
        ~Server();

        void runInThread();

        void newClient(ServerClientHandler* client);
        void removeClient(ServerClientHandler* client);
        void handlePacket(ServerClientHandler* client, Packet& packet);

    private:
        Poco::Thread thread;
        Poco::Net::ServerSocket* serverSocket;
        Poco::Net::SocketReactor* reactor;
        ConnectionAcceptor* acceptor;
        int port;

        //When creating a packet
        //it MUST be added to at least
        //one ServerClientHandler or
        //else it will be a memory leak
        //TODO: also save Packet* list
        //in Server, and on each update
        //check for zero refcounts
        Packet* createPacket(int id);

        void sendToAllClients(Packet* pak);

        int clientIdFactory;

        map<ServerClientHandler*,ServerClient*> clientList;

        typedef map<ServerClientHandler*,ServerClient*>::iterator clientIterator;
};
