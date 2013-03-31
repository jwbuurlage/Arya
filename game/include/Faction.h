#pragma once
#include "Arya.h"
#include <list>

using std::list;

using Arya::Object;

class Unit;
class Packet;
class GameSession;

class Faction
{
    private:
        friend class GameSession;
        GameSession* const session;

        Faction(int id, GameSession* session);
    public:
        //~Faction deletes all units it has
        //and unregisters itself at factory
        ~Faction();

        void addUnit(Unit* unit);

        void setColor(int col) { color = col; }
        vec3 getColor();

        list<Unit*>& getUnits() { return units; }

        void serialize(Packet& pk);
        void deserialize(Packet& pk);

        int getId() const { return id; }
        int getClientId() const { return clientId; }
        void setClientId(int id) { clientId = id; }

    private:
        const int id;
        int clientId;

        //!! It is important that a linked list is used here
        //because during the iteration over this container,
        //new units will be created and existing ones deleted
        //and iterators should not be invalidated after
        //such operations
        list<Unit*> units;
        int color;
};
