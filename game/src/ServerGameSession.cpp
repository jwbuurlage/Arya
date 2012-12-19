#include "../include/ServerGameSession.h"
#include "../include/ServerClient.h"
#include "../include/ServerClientHandler.h"
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

bool ServerGameSession::gameReady() const
{
    return true;
}
