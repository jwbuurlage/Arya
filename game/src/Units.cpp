#include "../include/Units.h"

using Arya::Root;

Unit::Unit(UnitInfo* inf)
{
    info = inf;
    object = 0;
    selected = false;
    targetPosition = vec2(0.0);
    speed = 30.0;
    yawspeed = 360.0f;
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

    float targeth;
    targeth = Root::shared().getScene()->getMap()->getTerrain()->heightAtGroundPosition(targetPosition.x, targetPosition.y);
    vec3 target(targetPosition.x, targeth, targetPosition.y);
    vec3 diff = target - object->getPosition();

    float newYaw = (180.0f/M_PI)*atan2(-diff.x, -diff.z);
    float oldYaw = object->getYaw();
    float yawDiff = newYaw - oldYaw;

    if( yawDiff > 180.0f ) yawDiff -= 360.0f;
    else if( yawDiff < -180.0f ) yawDiff += 360.0f;

    float deltaYaw = timeElapsed * yawspeed + 1.0f;
    if( (yawDiff >= 0 && yawDiff < deltaYaw) || (yawDiff <= 0 && yawDiff > -deltaYaw) )
    {
        //angle is small enough (less than 1 degree) so we can start walking now
        object->setYaw(newYaw);

        if(glm::length(diff) < 0.5) // arbitrary closeness...
        {
            object->setPosition(target);
            targetPosition = vec2(0.0);
            setIdle(true);
            return;
        }
        diff = glm::normalize(diff);

        vec3 newPosition = object->getPosition() + timeElapsed * (speed * diff);
        newPosition.y = Root::shared().getScene()->getMap()->getTerrain()->heightAtGroundPosition(newPosition.x, newPosition.z);
        object->setPosition(newPosition);
    }
    else
    {
        //Rotate
        if( yawDiff < 0 ) deltaYaw = -deltaYaw;
        object->setYaw( oldYaw + deltaYaw );
    }

}

void Unit::setIdle(bool idl)
{
    idle = idl;
    if(idle)
        object->setAnimation("stand");
    else
        object->setAnimation("run");
}

void Unit::setTargetPosition(vec2 target)
{
    targetPosition = target;
    setIdle(false);
}
