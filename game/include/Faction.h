#pragma once
#include "Arya.h"
#include <list>

using std::list;

using Arya::Object;

class Unit;
class Packet;

class Faction;

//Factory design pattern
class FactionFactory
{
    public:
        FactionFactory(){}
        virtual ~FactionFactory();

        //destory factions by calling delete on them
        Faction* createFaction(int id);
        Faction* getFactionById(int id);
    private:
        map<int,Faction*> factionMap;
        typedef map<int,Faction*>::iterator factionMapIterator;

        friend class Faction;
        void destroyFaction(int id);
};

class Faction
{
    private:
        friend class FactionFactory;
        FactionFactory* const factory;

        Faction(int id, FactionFactory* factory);
    public:
        ~Faction(); //unregisters itself at factory

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

        list<Unit*> units;
        int color;
};
