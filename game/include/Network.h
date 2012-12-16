#pragma once
#include <string>

using std::string;

class Server;
class Connection;
class Packet;

class Network
{
    public:
        Network();
        ~Network();

        void startServer();

        void connectToLobbyServer(string host, int port);
        void connectToSessionServer(string host, int port);

        //Currently this will block untill the packet
        //has been sent. This is because it usually
        //does not take any time (the OS has large enough buffers)
        //If it matters this can be changed in the future
        //However, then note: in that case the packet should be
        //added to a packet queue, and all the data of the packet
        //should be copied (since the caller will probably destroy the packet)
        //OR: let it be a pointer and tell the caller that this function
        //will destroy the packet
        void sendPacket(Packet& packet);

        //Call this every frame
        //or every 5 frames or so
        void update();

    private:
        Server* server;
        Connection* lobbyConnection;
        Connection* sessionConnection;
};

