#include "../include/common/GameLogger.h"
#include "../include/Faction.h"
#include "../include/Units.h"
#include "../include/Packet.h"
#include "../include/FactionColors.h"

Faction* FactionFactory::createFaction(int id)
{
    Faction* faction = getFactionById(id);
    if(faction)
    {
        GAME_LOG_WARNING("Trying to create faction with duplicate id (" << id << ")");
        return faction;
    }
    faction = new Faction(id, this);
    factionMap.insert(pair<int,Faction*>(faction->getId(),faction));
    return faction;
}

//Called from faction deconstructor
void FactionFactory::destroyFaction(int id)
{
    factionMapIterator iter = factionMap.find(id);
    if(iter == factionMap.end())
    {
        GAME_LOG_WARNING("Trying to destory unexisting faction id");
        return;
    }
    factionMap.erase(iter);
    return;
}

Faction* FactionFactory::getFactionById(int id)
{
    factionMapIterator iter = factionMap.find(id);
    if(iter == factionMap.end()) return 0;
    return iter->second;
}

Faction::Faction(int _id, FactionFactory* _factory) : factory(_factory), id(_id)
{
    color = 0;
}

Faction::~Faction()
{
    for(list<Unit*>::iterator it = units.begin(); 
            it != units.end();)
    {
        delete *it;
        it = units.erase(it);
    }
    factory->destroyFaction(id);
}

void Faction::addUnit(Unit* unit)
{
    unit->setTintColor(factionColors[color]);
    units.push_back(unit);
}

vec3 Faction::getColor()
{
    return factionColors[color];
}

void Faction::serialize(Packet& pk)
{
    pk << color;
}

void Faction::deserialize(Packet& pk)
{
    pk >> color;
}
