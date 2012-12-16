#pragma once

class Packet;

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
        EventManager(Network* network);
        ~EventManager();

        void addEventHandler(int eventId, EventHandler* handler);
        void removeEventHandler(int eventId, EventHandler* handler);

        void sendEvent(Packet& packet);
};
