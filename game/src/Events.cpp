#include "../include/Events.h"
#include "../include/Network.h"

EventManager::EventManager(Network* net)
{
    network = net;
}

EventManager::~EventManager()
{
}

void EventManager::addEventHandler(int eventId, EventHandler* handler)
{
}

void EventManager::removeEventHandler(int eventId, EventHandler* handler)
{
}

Event* EventManager::createEvent(int Id)
{
    if( Id >= 2000 ) return network->createSessionPacket(Id);
    else return network->createLobbyPacket(Id);
}

