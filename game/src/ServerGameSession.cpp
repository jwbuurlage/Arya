#include "../include/common/GameLogger.h"
#include "../include/Server.h"
#include "../include/ServerGameSession.h"
#include "../include/ServerClient.h"
#include "../include/ServerClientHandler.h"
#include "../include/EventCodes.h"
#include "../include/Units.h"
#include "../include/Map.h"
#include "Arya.h"

void ServerGameSession::addClient(ServerClient* client)
{
    if(client->getSession())
    {
        GAME_LOG_WARNING("ServerClient was already in a session and then added to another gamesession");
    }
    client->setSession(this);

    client->createFaction();
    client->createStartUnits();

    if(!clientList.empty())
    {
        //send the connect to all OTHER clients
        Packet* pak = server->createPacket(EVENT_CLIENT_CONNECTED);

        *pak << client->getClientId();

        Faction* faction = client->getFaction();
        *pak << faction->getId();
        faction->serialize(*pak);

        int unitCount = (int)faction->getUnits().size();

        *pak << unitCount;
        for(std::list<Unit*>::iterator uiter = faction->getUnits().begin(); uiter != faction->getUnits().end(); ++uiter)
        {
            *pak << (*uiter)->getId();
            (*uiter)->serialize(*pak);
        }

        sendToAllClients(pak);
    }

    //Then ADD this new client
    clientList.push_back(client);

    //Send the full game state only to the new client
    Packet* pak = server->createPacket(EVENT_GAME_FULLSTATE);

    *pak << getClientCount(); //player count

    //for each player:
    for(clientIterator iter = clientList.begin(); iter != clientList.end(); ++iter)
    {
        *pak << (*iter)->getClientId();

        Faction* faction = (*iter)->getFaction();
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
    client->handler->sendPacket(pak);
}

void ServerGameSession::removeClient(ServerClient* client)
{
    //TODO:
    //Lots of stuff like sending a message
    //to everyone about the client being disconnected
    //Distribute their resources accross allies
    //and so on
    int id = client->getClientId();
    for(clientIterator iter = clientList.begin(); iter != clientList.end(); ++iter)
    {
        if(*iter == client)
        {
            clientList.erase(iter);

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

bool ServerGameSession::gameReadyToLoad() const
{
    //TODO: Check if all players joined that needed to join
    return gameState == STATE_CREATED && clientList.size() >= 2;
}

bool ServerGameSession::gameReadyToStart() const
{
    return gameState == STATE_LOADING;
}

void ServerGameSession::startLoading()
{
    if(gameState != STATE_CREATED)
    {
        GAME_LOG_WARNING("startLoading() called when already loaded");
        return;
    }
    gameState = STATE_LOADING;

    Packet* pak = server->createPacket(EVENT_GAME_READY);
    sendToAllClients(pak);

    pak = server->createPacket(EVENT_GAME_FULLSTATE);

    //------------------------------------
    // Package structure:
    // + joinedCount
    //   - clientID
    //   - Serialized faction
    //      + UnitCount
    //      - Serialized unit 
    //------------------------------------

    *pak << getClientCount(); //player count

    //for each player:
    for(clientIterator iter = clientList.begin(); iter != clientList.end(); ++iter)
    {
        *pak << (*iter)->getClientId();

        Faction* faction = (*iter)->getFaction();
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
    sendToAllClients(pak);
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
                (*it)->update(elapsedTime, map);
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
