#include "../include/Server.h"
#include "../include/Packet.h"
#include "Arya.h"
#include <cstring>

#include "Poco/Exception.h"
#include "Poco/Net/NetException.h"
#include "Poco/NObserver.h"
#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketReactor.h"
#include "Poco/Net/SocketAcceptor.h"

using namespace Poco;
using namespace Poco::Net;

class NetworkClientHandler;

class ConnectionAcceptor : public SocketAcceptor<NetworkClientHandler>
{
    public:
        ConnectionAcceptor(ServerSocket& socket, SocketReactor& reactor) : SocketAcceptor(socket, reactor)
        {
            LOG_INFO("ConnectionAcceptor()");
        }

        ~ConnectionAcceptor()
        {
            LOG_INFO("~ConnectionAcceptor()");
        }
};

Server::Server()
{
    serverSocket = 0;
    reactor = 0;
    acceptor = 0;
    port = 1337;
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
    acceptor = new ConnectionAcceptor(*serverSocket, *reactor);

    thread.start(*reactor);
    LOG_INFO("Server started on port " << port);
}

class NetworkClientHandler
{
    public:
        NetworkClientHandler(StreamSocket& _socket, SocketReactor& _reactor) : socket(_socket), reactor(_reactor), bufferSizeTotal(1024)
        {
            LOG_INFO("NetworkClientHandler()");
            NObserver<NetworkClientHandler, ReadableNotification> readObserver(*this, &NetworkClientHandler::onReadable);
            NObserver<NetworkClientHandler, ShutdownNotification> shutdownObserver(*this, &NetworkClientHandler::onShutdown);
            reactor.addEventHandler(socket, readObserver);
            reactor.addEventHandler(socket, shutdownObserver);

            dataBuffer = new char[bufferSizeTotal+1];
            bytesReceived = 0;
        }

        ~NetworkClientHandler()
        {
            LOG_INFO("~NetworkClientHandler()");
            NObserver<NetworkClientHandler, ReadableNotification> readObserver(*this, &NetworkClientHandler::onReadable);
            NObserver<NetworkClientHandler, ShutdownNotification> shutdownObserver(*this, &NetworkClientHandler::onShutdown);
            reactor.removeEventHandler(socket, readObserver);
            reactor.removeEventHandler(socket, shutdownObserver);
            delete[] dataBuffer;
        }

        StreamSocket socket;
        SocketReactor& reactor;

        const int bufferSizeTotal;
        char* dataBuffer;
        int bytesReceived; //when a partial packet is in the buffer

        void onReadable(const AutoPtr<ReadableNotification>& notification)
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
                if(bytesReceived >= 8)
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
                        handlePacket(dataBuffer+8, packetSize - 8);
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

        void onShutdown(const AutoPtr<ShutdownNotification>& notification)
        {
            LOG_INFO("Shutdown notification");
            delete this;
        }

        void terminate()
        {
            socket.shutdown(); //send TCP shutdown
            socket.close();
            delete this;
        }

        void sendPacket(Packet* pak)
        {

        }

        void handlePacket(char* data, int packetSize)
        {
            LOG_INFO("Received Arya packet!");
        }
};

