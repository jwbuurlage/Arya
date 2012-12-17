#include "../include/ServerClientHandler.h"
#include "../include/Server.h"
#include "Arya.h"
#include <cstring>

#include "Poco/Exception.h"
#include "Poco/Net/NetException.h"
#include "Poco/NObserver.h"

ServerClientHandler::ServerClientHandler(StreamSocket& _socket, SocketReactor& _reactor) : socket(_socket), reactor(_reactor), bufferSizeTotal(1024)
{
    //Note that 'server' is not yet set here due to the way Poco works
    //The call to server->newClient is done in the connection acceptor.
    LOG_INFO("ServerClientHandler()");
    NObserver<ServerClientHandler, ReadableNotification> readObserver(*this, &ServerClientHandler::onReadable);
    NObserver<ServerClientHandler, WritableNotification> writeObserver(*this, &ServerClientHandler::onWritable);
    NObserver<ServerClientHandler, ShutdownNotification> shutdownObserver(*this, &ServerClientHandler::onShutdown);
    reactor.addEventHandler(socket, readObserver);
    reactor.addEventHandler(socket, writeObserver);
    reactor.addEventHandler(socket, shutdownObserver);

    dataBuffer = new char[bufferSizeTotal+1];
    bytesReceived = 0;
}

ServerClientHandler::~ServerClientHandler()
{
    LOG_INFO("~ServerClientHandler()");
    server->removeClient(this);
    NObserver<ServerClientHandler, ReadableNotification> readObserver(*this, &ServerClientHandler::onReadable);
    NObserver<ServerClientHandler, WritableNotification> writeObserver(*this, &ServerClientHandler::onWritable);
    NObserver<ServerClientHandler, ShutdownNotification> shutdownObserver(*this, &ServerClientHandler::onShutdown);
    reactor.removeEventHandler(socket, readObserver);
    reactor.removeEventHandler(socket, writeObserver);
    reactor.removeEventHandler(socket, shutdownObserver);
    delete[] dataBuffer;
}

void ServerClientHandler::onReadable(const AutoPtr<ReadableNotification>& notification)
{
    int n = 0;
    try
    {
        n = socket.receiveBytes(dataBuffer + bytesReceived, bufferSizeTotal - bytesReceived);
    }
    catch(TimeoutException& e)
    {
        LOG_WARNING("Timeout exception when reading socket!");
    }
    catch(NetException& e)
    {
        LOG_WARNING("Net exception when reading socket");
    }

    if(n <= 0)
    {
        LOG_INFO("Client closed connection");
        delete this;
    }
    else
    {
        bytesReceived += n;

        //Check if we received the packet header
        if(bytesReceived >= 12)
        {
            if( *(int*)dataBuffer != PACKETMAGICINT )
            {
                LOG_WARNING("Invalid packet header! Removing client");
                terminate();
                return;
            }
            int packetSize = *(int*)(dataBuffer + 4); //this is including the header
            if(packetSize > bufferSizeTotal)
            {
                LOG_WARNING("Packet does not fit in buffer. Possible hack attempt. Removing client. Packet size = " << packetSize);
                terminate();
                return;
            }
            if(bytesReceived >= packetSize)
            {
                handlePacket(dataBuffer, packetSize);
                //if there was more data in the buffer, move it
                //to the start of the buffer
                int extraSize = bytesReceived - packetSize;
                if(extraSize > 0)
                    memmove(dataBuffer, dataBuffer + packetSize, extraSize);
                bytesReceived = extraSize;
            }
        }
    }
}

void ServerClientHandler::onWritable(const AutoPtr<WritableNotification>& notification)
{
    for(vector< pair<Packet*,int> >::iterator pak = packets.begin(); pak != packets.end(); )
    {
        if(pak->first->markedForSend)
        {
            //If the full packet has been sent
            if( trySendPacketData( pak->first, pak->second ) )
            {
                pak->first->refCount--;
                if(pak->first->refCount == 0)
                    delete pak->first;
                pak = packets.erase(pak);
            }
            //always break because packets should be sent in order
            break;
        }
        else
        {
            ++pak;
        }
    }
}

void ServerClientHandler::onShutdown(const AutoPtr<ShutdownNotification>& notification)
{
    LOG_INFO("Shutdown notification");
    delete this;
}

void ServerClientHandler::terminate()
{
    socket.shutdown(); //send TCP shutdown
    socket.close();
}

void ServerClientHandler::sendPacket(Packet* pak)
{
    packets.push_back(pair<Packet*,int>(pak,0));
    pak->refCount++;
    pak->send(); //mark for send
}

bool ServerClientHandler::trySendPacketData(Packet* packet, int& bytesSent)
{
    int totalSize = packet->getSize();
    char* data = packet->getData();

    int n = 0;
    try
    {
        n = socket.sendBytes(data + bytesSent, totalSize - bytesSent);
    }
    catch(TimeoutException& e)
    {
        LOG_WARNING("Timeout exception when writing to socket! Msg: " << e.message());
        return false;
    }
    catch(NetException& e)
    {
        LOG_WARNING("Net exception when writing to socket. Msg: " << e.message());
        return false;
    }

    if(n<=0)
    {
        LOG_INFO("Server closed connection when writing to socket");
        terminate();
        return true;
    }
    else
    {
        bytesSent += n;
    }
    if( bytesSent >= totalSize )
        return true;
    return false;
}

void ServerClientHandler::handlePacket(char* data, int packetSize)
{
    Packet pak(data, packetSize);
    server->handlePacket(this, pak);
}

ConnectionAcceptor::ConnectionAcceptor(ServerSocket& socket, SocketReactor& reactor, Server* serv) : SocketAcceptor(socket, reactor), server(serv)
{
}

ConnectionAcceptor::~ConnectionAcceptor()
{
}

ServerClientHandler* ConnectionAcceptor::createServiceHandler(StreamSocket& socket)
{
    ServerClientHandler* handler = new ServerClientHandler(socket, *reactor());
    handler->server = server;
    server->newClient(handler);
    return handler;
}

