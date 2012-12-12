#include "../include/Units.h"

Unit::Unit()
{
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
