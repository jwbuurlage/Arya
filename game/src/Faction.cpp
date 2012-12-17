#include "../include/Faction.h"
#include "../include/Units.h"
#include "../include/Packet.h"
#include "../include/FactionColors.h"

Faction::Faction()
{
    id = -1;
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
    pk << id;
    pk << color;
}

void Faction::deserialize(Packet& pk)
{
    pk >> id;
    pk >> color;
}
