#pragma once
#include "Packet.h"
#include <vector>

#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"

using std::vector;
using std::pair;
using namespace Poco;
using namespace Poco::Net;

class Server;

class ServerClientHandler
{
    public:
        ServerClientHandler(StreamSocket& _socket, SocketReactor& _reactor);
        ~ServerClientHandler();

        //Add packet to the send queue
        void sendPacket(Packet* pak);

    private:
        StreamSocket socket;
        SocketReactor& reactor;
        Server* server; //is set on creation by ConnectionAcceptor
        friend class ConnectionAcceptor;

        const int bufferSizeTotal;
        char* dataBuffer;
        int bytesReceived; //when a partial packet is in the buffer

        void onReadable(const AutoPtr<ReadableNotification>& notification);
        void onWritable(const AutoPtr<WritableNotification>& notification);
        void onShutdown(const AutoPtr<ShutdownNotification>& notification);

        void terminate();

        void handlePacket(char* data, int packetSize);

        //Sends as much data as possible
        //If full packet has been sent
        //the packet refcount is decreased (happens in onWritable)
        //and the packet is deleted
        bool trySendPacketData(Packet* packet, int& bytesSent);

        //It can happen that the client can
        //currently not handle the data
        //In this case the packets get queued
        //The integer is the amount of bytes that has already been sent
        vector< pair<Packet*,int> > packets; //outgoing packet queue
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


