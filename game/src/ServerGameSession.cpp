#include "../include/common/GameLogger.h"
#include "../include/Server.h"
#include "../include/ServerGameSession.h"
#include "../include/ServerClient.h"
#include "../include/ServerClientHandler.h"
#include "../include/EventCodes.h"
#include "../include/Units.h"
#include "../include/Map.h"
#include "Arya.h"

ServerGameSession::ServerGameSession(Server* serv) : server(serv)
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
	for(factionIterator it = clientFactionList.begin(); it != clientFactionList.end(); ++it)
		delete *it;
	clientFactionList.clear();
}

void ServerGameSession::initialize()
{
	if(!clientFactionList.empty())
	{
		GAME_LOG_WARNING("ServerGameSession initialize called but factionlist was already populated. Removing old factions.");
		for(factionIterator it = clientFactionList.begin(); it != clientFactionList.end(); ++it)
			delete *it;
		clientFactionList.clear();
	}

	//create factions and start units
	for(int i = 0; i < gameInfo.playerCount; ++i)
	{
		//createFaction will register it to the list
		Faction* faction = createFaction();
		faction->setColor(gameInfo.players[i].color);

        int num = faction->getId() % 4;
        vec3 basePos( -250.0f + 500.0f * (num%2), 0.0f, -250.0f + 500.0f * (num/2) ); //one of the 4 corners of map
		for(int i = 0; i < 20; ++i)
		{
			Unit* unit = createUnit(0);
            unit->setPosition(basePos + vec3(20.0f*(i/10), 0.0f, 20.0f*(i%10)));
			faction->addUnit(unit);

			unit = createUnit(1);
            unit->setPosition(basePos + vec3(20.0f*(i/10) - 70.0f, 0.0f, 20.0f*(i%10) - 70.0f));
			faction->addUnit(unit);
		}
		clientFactionList.push_back(faction);
	}
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
	if(clientFactionList.empty())
	{
		GAME_LOG_ERROR("ServerGameSession addClient called before session was initialized!");
		return;
	}
    client->setSession(this);
	client->setFaction(clientFactionList[index]);
	clientFactionList[index]->setClientId(client->getClientId());
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

Packet* ServerGameSession::createFullStatePacket()
{
	Packet* pak = server->createPacket(EVENT_GAME_FULLSTATE);

	*pak << (int)clientFactionList.size();

	for(factionIterator iter = clientFactionList.begin(); iter != clientFactionList.end(); ++iter)
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
        GAME_LOG_WARNING("calling sendToAllClients but client list is empty. This means the packet is NOT added to any queue and the pointer is probably lost");
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

void ServerGameSession::update(float elapsedTime)
{
    for(clientIterator clientIter = clientList.begin(); clientIter != clientList.end(); ++clientIter)
    {
        Faction* faction = (*clientIter)->getFaction();
        if(!faction) continue;

        for(list<Unit*>::iterator it = faction->getUnits().begin();
                it != faction->getUnits().end(); )
        {
            if((*it)->obsolete() && (*it)->readyToDelete())
            {
                delete *it;
                it = faction->getUnits().erase(it);
            }
            else
            {
                (*it)->serverUpdate(elapsedTime, map, this);
                ++it;
            }
        }
    }
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
                        vec2 targetPos;
                        packet >> unitId >> targetPos;

                        Unit* unit = getUnitById(unitId);
                        if(unit)
                        {
                            //TODO: check if valid movement
                            unit->setTargetPosition(targetPos);
                            validUnits.push_back(unit);
                        }
                    }

                    if(!validUnits.empty())
                    {
                        Packet* outPak = server->createPacket(EVENT_MOVE_UNIT);

                        *outPak << (int)validUnits.size();
                        for(unsigned int i = 0; i < validUnits.size(); ++i)
                        {
                            *outPak << validUnits[i]->getId() << validUnits[i]->getTargetPosition();
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
    if(!map)
    {
        map = new Map;
        if(!map->initHeightData())
        {
            delete map;
            map = 0;
        }
    }
}
