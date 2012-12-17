#pragma once
#include "../include/Faction.h"

class ServerClientHandler;
class ConnectionAcceptor;
class Packet;
class Server;

class Faction;


//Any connected session client.
//May not be in a game yet
class ServerClient
{
    public:
        ServerClient(Server* serv, ServerClientHandler* cl) : handler(cl)
        {
            server = serv;
            clientId = -1;
            faction = 0;
        }
        ~ServerClient()
        {
            if(faction) delete faction;
        }
        ServerClientHandler* const handler; //used to send packets

        int getClientId() const { return clientId; }
        void setClientId(int id){ clientId = id; }

        //clientId is assumed to be set already
        void createFaction();

        Faction* getFaction() const { return faction; }

        void createStartUnits();

    private:
        int clientId; //-1 indicates not joined yet
        Faction* faction;

        Server* server;
};


