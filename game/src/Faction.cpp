#include "../include/Faction.h"
#include "../include/Units.h"

Faction::Faction()
{

}

Faction::~Faction()
{

}

void Faction::addUnit(Unit* unit)
{
    units.push_back(unit);
}
