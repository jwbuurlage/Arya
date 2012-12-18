#include "../include/Events.h"
#include "../include/Network.h"
#include "Arya.h"
#include <algorithm>

using std::pair;
using std::make_pair;

EventManager::EventManager(Network* net)
{
    network = net;
}

EventManager::~EventManager()
{
}

void EventManager::addEventHandler(int eventId, EventHandler* handler)
{
    eventHandlers.insert(make_pair(eventId,handler));
}

void EventManager::removeEventHandler(int eventId, EventHandler* handler)
{
    pair<handlerIterator, handlerIterator> range = eventHandlers.equal_range(eventId);
    for(handlerIterator iter = range.first; iter != range.second; ++iter)
    {
        if( iter->second == handler )
        {
            eventHandlers.erase(iter);
        }
    }
}

Event& EventManager::createEvent(int Id)
{
    if( Id >= 2000 ) return *network->createSessionPacket(Id);
    else return *network->createLobbyPacket(Id);
}

void EventManager::handlePacket(Packet& packet)
{
    int id = packet.getId();
    pair<handlerIterator, handlerIterator> range = eventHandlers.equal_range(id);
    if(range.first == range.second)
    {
        LOG_WARNING("Event id " << id << " received but no handler registered.");
    }
    else
    {
        for(handlerIterator iter = range.first; iter != range.second; ++iter)
        {
            iter->second->handleEvent(packet);
        }
    }
}
