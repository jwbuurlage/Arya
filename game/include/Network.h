#pragma once
#include <string>

using std::string;

class EventManager;
class Server;
class Connection;
class Packet;

class Network
{
    public:
        Network();
        ~Network();

        void setPacketHandler(EventManager* handler);

        //starts thread
        void startServer();
        //runs in same thread
        void runServer();

        void connectToLobbyServer(string host, int port);
        void connectToSessionServer(string host, int port);

        //Packets are sent by packet->Send()
        //which marks it for sending on next update
        Packet* createLobbyPacket(int id);
        Packet* createSessionPacket(int id);

        //Call this every frame
        //or every 5 frames or so
        void update();

    private:
        Server* server;
        Connection* lobbyConnection;
        Connection* sessionConnection;
        EventManager* eventHandler;
};

