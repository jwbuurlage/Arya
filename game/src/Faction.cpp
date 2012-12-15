#include "../include/Faction.h"
#include "../include/Units.h"

Faction::Faction()
{

}

Faction::~Faction()
{
    for(int i = 0; i < units.size(); ++i)
        delete units[i];
}

void Faction::addUnit(Unit* unit)
{
    units.push_back(unit);
}
