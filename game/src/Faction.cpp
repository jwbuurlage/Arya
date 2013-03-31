#include "../include/common/GameLogger.h"
#include "../include/Faction.h"
#include "../include/GameSession.h"
#include "../include/Units.h"
#include "../include/Packet.h"
#include "../include/FactionColors.h"

Faction::Faction(int _id, GameSession* _session) : session(_session), id(_id)
{
	clientId = -1;
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
    session->destroyFaction(id);
}

void Faction::addUnit(Unit* unit)
{
    unit->setTintColor(factionColors[color]);
    unit->setFactionId(id);
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
