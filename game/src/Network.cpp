#include "../include/Network.h"
#include "../include/Server.h"
#include "Arya.h"
#include "Poco/Net/StreamSocket.h"

using namespace Poco;
using namespace Poco::Net;

class Connection
{
    public:
        Connection() : socket(new StreamSocket)
        {
            connected = false;
            connecting = false;
        }

        ~Connection()
        {
            delete socket;
        }

        void connect(string host, int port)
        {
            if(connected || connecting) socket->close();
            socket->connectNB(SocketAddress(host,port));
            connected = false;
            connecting = true;
            LOG_INFO("Started connection to " << host);
        }

        void update()
        {
            if(connecting)
            {
                if(socket->poll(0, StreamSocket::SELECT_WRITE))
                {
                    LOG_INFO("Connected to server!");
                    connected = true;
                    connecting = false;
                }
            }
            else if(connected)
            {
                if(socket->poll(0, StreamSocket::SELECT_READ))
                {
                    LOG_INFO("Server sent data");
                }
            }
        }

        void sendPacket(Packet& packet)
        {

        }

        bool connected;
        bool connecting;
        StreamSocket* const socket; //const so it can not be made zero
};

Network::Network()
{
    server = 0;
    lobbyConnection = new Connection;
    sessionConnection = new Connection;
}

Network::~Network()
{
    if(server) delete server;
    delete lobbyConnection;
    delete sessionConnection;
}

void Network::startServer()
{
    if(server) delete server;
    server = new Server;
    server->runInThread();
}

void Network::connectToLobbyServer(string host, int port)
{
    lobbyConnection->connect(host,port);
}

void Network::connectToSessionServer(string host, int port)
{
    sessionConnection->connect(host,port);
}

void Network::update()
{
    lobbyConnection->update();
    sessionConnection->update();
}
