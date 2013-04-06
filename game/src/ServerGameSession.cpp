#include "../include/common/GameLogger.h"
#include "../include/Server.h"
#include "../include/ServerGameSession.h"
#include "../include/ServerClient.h"
#include "../include/ServerClientHandler.h"
#include "../include/EventCodes.h"
#include "../include/Units.h"
#include "../include/Faction.h"
#include "../include/Map.h"
#include "../include/MapInfo.h"
#include "Arya.h"

ServerGameSession::ServerGameSession(Server* serv) : GameSession(serv->getScripting(), true), server(serv)
{
	gameStarted = false;
	idFactory = 1;

	gameInfo.playerCount = 0;
	for(int i = 0; i < MAX_PLAYER_COUNT; ++i)
	{
		gameInfo.players[i].accountId = 0;
		gameInfo.players[i].sessionHash = 0;
		gameInfo.players[i].slot = 0;
		gameInfo.players[i].color = 0;
		gameInfo.players[i].team = 0;
	}

	map = 0;
	initMap();
}

ServerGameSession::~ServerGameSession()
{
    //Deleting the factions will cause all units to be deleted
	for(factionIterator it = factionList.begin(); it != factionList.end(); ++it)
		delete *it;
	factionList.clear();
}

void ServerGameSession::initialize()
{
	if(!factionList.empty())
	{
		GAME_LOG_WARNING("ServerGameSession initialize called but factionlist was already populated. Removing old factions.");
		for(factionIterator it = factionList.begin(); it != factionList.end(); ++it)
			delete *it;
		factionList.clear();
	}

	//create factions
	for(int i = 0; i < gameInfo.playerCount; ++i)
	{
		Faction* faction = createFaction();
		faction->setColor(gameInfo.players[i].color);
    }

    //Note that the script can also create factions at onLoad

    //TODO: change this theMap thing to something else, see MapInfo.h
    theMap->onLoad(this);
    for(int i = 0; i < gameInfo.playerCount; ++i)
        theMap->onLoadFaction(this, factionList[i]->getId(), gameInfo.players[i].slot);
}

Faction* ServerGameSession::createFaction()
{
    Faction* faction = GameSession::createFaction(getNewId());
    factionList.push_back(faction);
    return faction;
}

void ServerGameSession::addClient(ServerClient* client, int index)
{
	if(index < 0 || index >= gameInfo.playerCount)
	{
		GAME_LOG_WARNING("ServerGameSession addClient called with invalid index!");
		return;
	}
    if(client->getSession())
    {
        GAME_LOG_WARNING("ServerClient was already in a session and then added to another gamesession");
    }
	if(factionList.empty())
	{
		GAME_LOG_ERROR("ServerGameSession addClient called before session was initialized!");
		return;
	}
    client->setSession(this);
	client->setFaction(factionList[index]);
	factionList[index]->setClientId(client->getClientId());
    clientList.push_back(client);
    //Send the full game state (only to the new client)
	client->handler->sendPacket(createFullStatePacket());
}

void ServerGameSession::removeClient(ServerClient* client)
{
    //TODO:
    //Lots of stuff like sending a message
    //to everyone about the client being disconnected
	//
	//Let the faction remain intact in case the player reconnects
    int id = client->getClientId();
    Faction* faction = client->getFaction();
    if(faction) faction->setClientId(-1);
    for(clientIterator iter = clientList.begin(); iter != clientList.end(); ++iter)
    {
        if(*iter == client)
        {
            clientList.erase(iter);

			//if there are remaining clients, let them know
            if(!clientList.empty())
            {
                Packet* pak = server->createPacket(EVENT_CLIENT_DISCONNECTED);
                *pak << id;
                sendToAllClients(pak);
            }

            break;
        }
    }
}

void ServerGameSession::sendUnitSpawnPacket(Unit* unit)
{
    if(clientList.empty()) return;
    if(unit->getFactionId() == -1) return;
    Packet* pak = server->createPacket(EVENT_UNIT_SPAWNED);
    *pak << unit->getFactionId();
    *pak << unit->getId();
    unit->serialize(*pak);
    sendToAllClients(pak);
}

Packet* ServerGameSession::createFullStatePacket()
{
	Packet* pak = server->createPacket(EVENT_GAME_FULLSTATE);

	*pak << (int)factionList.size();

	for(factionIterator iter = factionList.begin(); iter != factionList.end(); ++iter)
	{
		Faction* faction = *iter;

		*pak << faction->getClientId();
		*pak << faction->getId();

		faction->serialize(*pak);

		int unitCount = (int)faction->getUnits().size();
		*pak << unitCount;
		for(std::list<Unit*>::iterator uiter = faction->getUnits().begin(); uiter != faction->getUnits().end(); ++uiter)
		{
			*pak << (*uiter)->getId();
			(*uiter)->serialize(*pak);
		}
	}
	return pak;
}

Packet* ServerGameSession::createPacket(int id)
{
	return server->createPacket(id);
}

void ServerGameSession::sendToAllClients(Packet* pak)
{
    if(clientList.empty())
    {
        GAME_LOG_WARNING("calling sendToAllClients but client list is empty. deleting packet.");
        server->deletePacket(pak);
        return;
    }
    for(clientIterator iter = clientList.begin(); iter != clientList.end(); ++iter)
    {
        (*iter)->handler->sendPacket(pak);
    }
}

void ServerGameSession::startGame()
{

}

void ServerGameSession::getUnitsNearLocation(float x, float z, float distance, vector<Unit*>& result)
{
    result.clear();
	for(factionIterator fac = factionList.begin(); fac != factionList.end(); ++fac)
	{
		Faction* faction = *fac;
        for(list<Unit*>::iterator it = faction->getUnits().begin(); it != faction->getUnits().end(); ++it)
        {
            Unit* unit = *it;
            if( glm::distance(unit->getPosition2(), vec2(x,z)) < distance )
                result.push_back(unit);
        }
    }
    return;
}

void ServerGameSession::update(float elapsedTime)
{
	for(factionIterator fac = factionList.begin(); fac != factionList.end(); ++fac)
	{
		Faction* faction = *fac;

        if(faction->getUnits().empty()) continue;

        bool lost = false;

        for(list<Unit*>::iterator it = faction->getUnits().begin();
                it != faction->getUnits().end(); )
        {
            Unit* unit = *it;

            if(unit->readyToDelete())
            {
                delete unit;
                it = faction->getUnits().erase(it);
            }
            else
            {
                //during this update, many events can occur like
                //units dieing, units being created, players losing, etc
                //TODO: check if this can result in unexpected behaviour
                // - creating units: get added to the end of list<Unit*>
                //   the std::list specification states that iterators are
                //   still valid after a push_back
                //   note that the new unit will get serverUpdate() in the
                //   same frame that it was created, so its timing will
                //   be different?
                // - removing units: this may never be done within serverUpdate
                //   the only way to remove units is by calling unit->markForDelete()
                //   so that this loop will remove it
                // - big events like a full faction that loses all its units
                //   ???????? solution needed ??????
                unit->update(elapsedTime);
                ++it;
            }
        }

        //all units of a faction have died during this loop
        //player loses
        if(lost)
        {
            if(clientList.empty()==false)
            {
                Packet *pak = server->createPacket(EVENT_PLAYER_DEFEAT);
                *pak << faction->getId();
                sendToAllClients(pak);
            }
            for(list<Unit*>::iterator it = faction->getUnits().begin(); it != faction->getUnits().end(); ++it)
            {
                (*it)->markForDelete();
            }
        }
    }

    theMap->onUpdate(this, elapsedTime);
}

void ServerGameSession::handlePacket(ServerClient* client, Packet& packet)
{
    Faction* faction = client->getFaction();
    switch(packet.getId())
    {
		case EVENT_GAME_FULLSTATE_REQUEST:
			client->handler->sendPacket(createFullStatePacket());
			break;
        case EVENT_MOVE_UNIT_REQUEST:
            {
                if(faction)
                {
                    int count;
                    packet >> count;

                    vector<Unit*> validUnits;
                    validUnits.reserve(count);
                    for(int i = 0; i < count; ++i)
                    {
                        int unitId;
                        int nodeCount;
                        vector<vec2> pathNodes;
                        packet >> unitId >> nodeCount;
                        for(int i = 0; i < nodeCount; ++i)
                        {
                            vec2 temp;
                            packet >> temp;
                            pathNodes.push_back(temp);
                        }
                        Unit* unit = getUnitById(unitId);
                        if(unit && nodeCount >= 1)
                        {
                            //TODO: check if valid movement
                            unit->setTargetPath(pathNodes);
                            validUnits.push_back(unit);
                        }
                    }

                    if(!validUnits.empty())
                    {
                        Packet* outPak = server->createPacket(EVENT_MOVE_UNIT);

                        *outPak << (int)validUnits.size();
                        for(unsigned int i = 0; i < validUnits.size(); ++i)
                        {
                            *outPak << validUnits[i]->getId();
                            *outPak << (int)validUnits[i]->getTargetPath().size();
                            for(unsigned int j = 0; j < validUnits[i]->getTargetPath().size(); ++j)
                            {
                                *outPak << validUnits[i]->getTargetPath()[j];
                            }
                        }
                        sendToAllClients(outPak);
                    }
                }
            }
            break;
        case EVENT_ATTACK_MOVE_UNIT_REQUEST:
            {
                if(faction)
                {
                    int count;
                    packet >> count;

                    vector<Unit*> validUnits;
                    validUnits.reserve(count);
                    for(int i = 0; i < count; ++i)
                    {
                        int unitId, targetUnitId;
                        packet >> unitId >> targetUnitId;
                        Unit* unit = getUnitById(unitId);
                        Unit* target = getUnitById(targetUnitId);
                        if(unit && target)
                        {
                            //TODO: check if valid movement
                            unit->setTargetUnit(target);
                            validUnits.push_back(unit);
                        }
                    }

                    if(!validUnits.empty())
                    {
                        Packet* outPak = server->createPacket(EVENT_ATTACK_MOVE_UNIT);

                        *outPak << (int)validUnits.size();
                        for(unsigned int i = 0; i < validUnits.size(); ++i)
                        {
                            *outPak << validUnits[i]->getId() << validUnits[i]->getTargetUnit()->getId();
                        }
                        sendToAllClients(outPak);
                    }
                }
            }
            break;
        default:
            break;
    }
    return;
}

void ServerGameSession::initMap()
{
    //TODO: The map is now reloaded for every game session. Please.
    //Also, the server does not even need the map right now.
    return;
    if(!map)
    {
        map = new Map(theMap);

        if(!map->initHeightData())
        {
            delete map;
            map = 0;
            return;
        }
    }
}
