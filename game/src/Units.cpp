#include "../include/Units.h"

using Arya::Root;

Unit::Unit(UnitInfo* inf)
{
    info = inf;
    object = 0;
    selected = false;
    targetPosition = vec2(0.0);
    velocity = vec2(0.5);
    idle = true;
}

Unit::~Unit()
{
    // todo decunstruct the obj
}

void Unit::setObject(Object* obj)
{
    object = obj;
}

void Unit::update(float timeElapsed)
{
    if(idle)
        return;

    float h;
    vec2 currentPosition = vec2(object->getPosition().x, object->getPosition().z);
    vec2 diff = (targetPosition - currentPosition);

    if(glm::length(diff) < 0.2) // arbitrary closeness...
    {
        h = Root::shared().getScene()->getMap()->getTerrain()->heightAtGroundPosition(targetPosition.x, targetPosition.y);
        object->setPosition(vec3(targetPosition.x, h, targetPosition.y));
        targetPosition = vec2(0.0);
        idle = true;
        LOG_INFO("Reached destination");
        return;
    }

    vec2 newPosition = currentPosition + timeElapsed * (velocity * diff);
    h = Root::shared().getScene()->getMap()->getTerrain()->heightAtGroundPosition(newPosition.x, newPosition.y);

    object->setPosition(vec3(newPosition.x, h, newPosition.y));
}

void Unit::setTargetPosition(vec2 target)
{
    targetPosition = target;
    idle = false;
}
