#pragma once

#include "Poco/Thread.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"

class ConnectionAcceptor;

class Server
{
    public:
        Server();
        ~Server();

        void runInThread();

    private:
        Poco::Thread thread;
        Poco::Net::ServerSocket* serverSocket;
        Poco::Net::SocketReactor* reactor;
        ConnectionAcceptor* acceptor;

        int port;
};
