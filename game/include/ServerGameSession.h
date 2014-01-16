// Serverside game session
//
//- Lobby decides that a new game starts
//- Lobby connects to this server, and server.cpp creates
//	a new ServerGameSession object and uses setGameInfo
//	to set the game settings as well as the sessionhash
//	which also serves as some sort of password for the session
//- ServerGameSession loads the map and other assets
//- Clients connect to the server and server.cpp forwards
//	these clients to ServerGameSession (addClient)
//- ServerGameSession sends each client the full game state
//	which includes positions of all units etc
//-	The client sends EVENT_CLIENT_READY when done loading
//- When all clients are done, ServerGameSession sends
//	EVENT_GAME_START and the game timer starts

#pragma once
#include <vector>
#include "GameSession.h"

using std::vector;

class Map;
class Unit;
class Faction;
class Server;
class ServerClient;
class Packet;

const int MAX_PLAYER_COUNT = 8;

struct GameInfo
{
	//Game info
	
	//gametype
	//map
	//gamespeed
	//technology enables/disables

	//Player info
	int playerCount;
	struct PlayerInfo
	{
		int accountId;
		int sessionHash; //password to get into this session
		char slot; //spawn position
		char color;
		char team;
	} players[MAX_PLAYER_COUNT];
};

class ServerGameSession : public GameSession
{
    public:
		//Server class creates a new ServerGameSession
		//then uses getGameInfo() to set the game info
		//and then calls initialize()

        ServerGameSession(Server* serv);
        ~ServerGameSession();

		GameInfo& getGameInfo() { return gameInfo; }

		void initialize();

		//index is the index into the GameInfo players array
        void addClient(ServerClient* client, int index);
        void removeClient(ServerClient* client);

        void update(int elapsedTime); //in ms

		Packet* createFullStatePacket();

        void handlePacket(ServerClient* client, Packet& packet);

        int getNewId() { return idFactory++; }
        Unit* createUnit(int type){ return GameSession::createUnit(getNewId(),type); }
        Faction* createFaction();

        //sends to all clients
        //The reason that this is not inside createUnit is
        //that we first want to set unit properties like position
        //before sending the packet
        void sendUnitSpawnPacket(Unit* unit);

        //connected clients, can be spectators
        unsigned int getClientCount() const { return clientList.size(); }

		Packet* createPacket(int id);
        void sendToAllClients(Packet* pak);

		bool isGameStarted() const { return gameStarted; }
        void startGame();

        void getUnitsNearLocation(float x, float z, float distance, vector<Unit*>& result);
    private:
        Server* const server;
        int idFactory;

        //List of connected clients. Could also be spectators,
        //does not have to be an actual faction.
        vector<ServerClient*> clientList;
        typedef vector<ServerClient*>::iterator clientIterator;

		//!! Important:
		//the order of this list corresponds to the
		//order of the players in GameInfo
        //The first 'gameInfo.playerCount' factions correspond to the
        //players, and the other factions are neutrals or created by scripts
		vector<Faction*> factionList;
		typedef vector<Faction*>::iterator factionIterator;

		GameInfo gameInfo;
        //Game state
		//The lobby server will decide the settings
		//As soon as this game session is created,
		//all game-setting choices have been made.
		//
		//The game state starts out as LOADING
		//Newly connected clients will get sent a game-state (unit positions etc)
		//Every client then starts loading and will send a LOADED event
		//when done. When all clients have sent this, the server decides
		//to start the timer (maybe a second delay after last load)
		//and the state is changed to STARTED and the game timer starts
		bool gameStarted;

        void initMap();
};
