#pragma once

#include "EventCodes.h"
#include "Packet.h"
#include <map>

using std::multimap;

typedef Packet Event;

class EventHandler
{
    public:
        //Id is obtained by packet.getId()
        virtual void handleEvent(Packet& packet) = 0;
};

class Network;

//Can send events, and dispatches incoming events to registered handlers
class EventManager
{
    public:
        EventManager(Network* net);
        ~EventManager();

        void addEventHandler(int eventId, EventHandler* handler);
        void removeEventHandler(int eventId, EventHandler* handler);
        //Remove from all registered events at once:
        void removeEventHandler(EventHandler* handler);

        //Sending events is done by event->Send();
        //which marks it for sending on next update
        Event& createEvent(int Id);

        //Called by network on incoming packet
        void handlePacket(Packet& packet);

    private:
        Network* network;

        multimap<int, EventHandler*> eventHandlers;

        typedef multimap<int,EventHandler*>::iterator handlerIterator;
};
