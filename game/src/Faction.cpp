#include "../include/Faction.h"
#include "../include/Units.h"

Faction::Faction()
{
    color = vec3(1.0);
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
    unit->getObject()->setTintColor(color);
    units.push_back(unit);
}
