#include "../include/Faction.h"
#include "../include/Units.h"

Faction::Faction()
{
    color = vec3(1.0);
}

Faction::~Faction()
{
    for(int i = 0; i < units.size(); ++i)
        delete units[i];
}

void Faction::addUnit(Unit* unit)
{
    unit->getObject()->setTintColor(color);
    units.push_back(unit);
}
