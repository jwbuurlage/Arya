#pragma once
#include <vector>

using std::vector;

class ServerClient;
class Packet;

class ServerGameSession
{
    public:
        ServerGameSession()
        {
            gameState = STATE_LOADING;
        }
        ~ServerGameSession()
        {
        }

        void addClient(ServerClient* client);
        void removeClient(ServerClient* client);

        unsigned int getClientCount() const { return clientList.size(); }

        void sendToAllClients(Packet* pak);

        //When enough clients joined
        bool gameReady() const;
    private:
        vector<ServerClient*> clientList;
        typedef vector<ServerClient*>::iterator clientIterator;

        //Game state
        //1. LOADING: clients start connecting
        //2. READY: when enough clients connected
        //       the server starts a game session
        //3. STARTED: the game timer starts
        //      units can walk etc
        enum{ STATE_LOADING = 0,
            STATE_READY = 1,
            STATE_STARTED = 2
        } gameState;

};
