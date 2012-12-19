#include "../include/Server.h"
#include "../include/ServerGameSession.h"
#include "../include/ServerClient.h"
#include "../include/ServerClientHandler.h"
#include "../include/EventCodes.h"
#include "../include/Units.h"
#include "Arya.h"

void ServerGameSession::addClient(ServerClient* client)
{
    if(client->getSession())
    {
        LOG_WARNING("ServerClient was already in a session and then added to another gamesession");
    }
    client->setSession(this);
    clientList.push_back(client);
}

void ServerGameSession::removeClient(ServerClient* client)
{
    //TODO:
    //Lots of stuff like sending a message
    //to everyone about the client being disconnected
    //Distribute their resources accross allies
    //and so on
    for(clientIterator iter = clientList.begin(); iter != clientList.end(); ++iter)
    {
        if(*iter == client)
        {
            clientList.erase(iter);
            break;
        }
    }
}

void ServerGameSession::sendToAllClients(Packet* pak)
{
    for(clientIterator iter = clientList.begin(); iter != clientList.end(); ++iter)
    {
        (*iter)->handler->sendPacket(pak);
    }
}

bool ServerGameSession::gameReadyToLoad() const
{
    //TODO: Check if all players joined that needed to join
    return gameState == STATE_CREATED;
}

bool ServerGameSession::gameReadyToStart() const
{
    return gameState == STATE_LOADING;
}

void ServerGameSession::startLoading()
{
    if(gameState != STATE_CREATED)
    {
        LOG_WARNING("startLoading() called when already loaded");
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
        (*iter)->createFaction();
        (*iter)->createStartUnits();

        *pak << (*iter)->getClientId();

        Faction* faction = (*iter)->getFaction();
        faction->serialize(*pak);

        int unitCount = (int)faction->getUnits().size();

        *pak << unitCount;
        for(std::list<Unit*>::iterator uiter = faction->getUnits().begin(); uiter != faction->getUnits().end(); ++uiter)
            (*uiter)->serialize(*pak);
    }
    sendToAllClients(pak);
}

void ServerGameSession::startGame()
{

}

void ServerGameSession::handlePacket(ServerClient* client, Packet& packet)
{
    switch(packet.getId())
    {
        case EVENT_MOVE_UNIT_REQUEST:
            {
                LOG_INFO("Move request accepted");
                Faction* faction = client->getFaction();
                if(faction)
                {
                    Packet* outPak = server->createPacket(EVENT_MOVE_UNIT);
                    *outPak << faction->getId();

                    int count;
                    packet >> count;
                    *outPak << count;
                    for(int i = 0; i < count; ++i)
                    {
                        int unitId;
                        vec2 targetPos;
                        packet >> unitId >> targetPos;
                        *outPak << unitId << targetPos;
                    }
                    sendToAllClients(outPak);
                }
            }
            break;
        default:
            break;
    }
    return;
}
