#pragma once

class Packet;

typedef Packet Event;

class EventHandler
{
    public:
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

        //Sending events is done by event->Send();
        //which marks it for sending on next update
        Event* createEvent(int Id);

    private:
        Network* network;
};
