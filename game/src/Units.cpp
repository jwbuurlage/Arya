#include "../include/Units.h"

Unit::Unit(UnitInfo* inf)
{
    info = inf;
    object = 0;
}

Unit::~Unit()
{
    // todo decunstruct the obj
}

void Unit::setObject(Object* obj)
{
    object = obj;
}
