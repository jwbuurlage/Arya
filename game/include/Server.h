#pragma once

#include "Poco/Thread.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketReactor.h"

class ConnectionAcceptor;
class Packet;

class Server
{
    public:
        Server();
        ~Server();

        void runInThread();

        void handlePacket(Packet& packet);

    private:
        Poco::Thread thread;
        Poco::Net::ServerSocket* serverSocket;
        Poco::Net::SocketReactor* reactor;
        ConnectionAcceptor* acceptor;

        int port;
};
