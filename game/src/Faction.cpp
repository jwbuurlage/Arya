#include "../include/Faction.h"
#include "../include/Units.h"

Faction::Faction()
{

}

Faction::~Faction()
{

}

void Faction::addUnit(Object* obj)
{
    Unit* unit = new Unit;
    unit->setObject(obj);
    units.push_back(unit);
}
