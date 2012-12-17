#pragma once
#include "Packet.h"

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"

using namespace Poco;
using namespace Poco::Net;

class Server;

class ServerClientHandler
{
    public:
        ServerClientHandler(StreamSocket& _socket, SocketReactor& _reactor);
        ~ServerClientHandler();

        StreamSocket socket;
        SocketReactor& reactor;
        Server* server; //is set on creation by ConnectionAcceptor

        const int bufferSizeTotal;
        char* dataBuffer;
        int bytesReceived; //when a partial packet is in the buffer

        void onReadable(const AutoPtr<ReadableNotification>& notification);

        void onShutdown(const AutoPtr<ShutdownNotification>& notification);

        void terminate();

        void sendPacket(Packet* packet);

        void handlePacket(char* data, int packetSize);
};

class ConnectionAcceptor : public SocketAcceptor<ServerClientHandler>
{
    public:
        ConnectionAcceptor(ServerSocket& socket, SocketReactor& reactor, Server* serv);

        ~ConnectionAcceptor();

        ServerClientHandler* createServiceHandler(StreamSocket& socket);

    private:
        Server* server;
};


