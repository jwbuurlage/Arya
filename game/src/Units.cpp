#include "../include/Units.h"
#include <math.h>

#ifdef _WIN32
#define M_PI 3.14159265358979323846
#endif

using Arya::Root;

Unit::Unit(UnitInfo* inf)
{
    info = inf;
    object = 0;
    selected = false;
    targetPosition = vec2(0.0f);
    speed = 30.0f;
    yawspeed = 720.0f;
    unitState = UNIT_IDLE;
    targetUnit = 0;

    health = 100;
    damage = 20;

    attackSpeed = 1.0f; 
    // set high enough tsla, so we can start attacking
    timeSinceLastAttack = attackSpeed + 1.0f;

    dyingTime = 0.0f;

    refCount = 0;
}

Unit::~Unit()
{
    object->setObsolete();
}

void Unit::setObject(Object* obj)
{
    object = obj;
}

void Unit::update(float timeElapsed)
{
    if(unitState == UNIT_IDLE)
        return;

    if(unitState == UNIT_DYING)
    {
        dyingTime += timeElapsed;
        return;
    }

    if(unitState == UNIT_ATTACKING || 
            unitState == UNIT_ATTACKING_OUT_OF_RANGE)
    {
        if(!targetUnit) {
            LOG_WARNING("Attacking, but no target unit");
            setUnitState(UNIT_IDLE);
            return;
        }

        if(!(targetUnit->isAlive()) || targetUnit->obsolete())
        {
            targetUnit->release();
            targetUnit = 0;
            setUnitState(UNIT_IDLE);
            return;
        }

        if(glm::distance(object->getPosition(), targetUnit->getObject()->getPosition())
                < targetUnit->getInfo()->radius) {
            if(unitState != UNIT_ATTACKING)
                setUnitState(UNIT_ATTACKING);

            if(timeSinceLastAttack > attackSpeed)
            {
                // make one attack
                targetUnit->receiveDamage(damage, this);
                if(!(targetUnit->isAlive()))
                {
                    targetUnit->release();
                    targetUnit = 0;
                    setUnitState(UNIT_IDLE);
                    timeSinceLastAttack = attackSpeed + 1.0f;
                    return;
                }
                timeSinceLastAttack = 0.0f;
            }
            else
                timeSinceLastAttack += timeElapsed;

            return;
        }
        else {
            if(unitState != UNIT_ATTACKING_OUT_OF_RANGE)
                setUnitState(UNIT_ATTACKING_OUT_OF_RANGE);
            targetPosition = vec2(targetUnit->getObject()->getPosition().x,
                            targetUnit->getObject()->getPosition().z);
        }
    }

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
            setUnitState(UNIT_IDLE);
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

void Unit::setUnitState(UnitState state)
{
    unitState = state;

    switch(unitState)
    {
        case UNIT_IDLE:
            object->setAnimation("stand");
            break;

        case UNIT_RUNNING:
            object->setAnimation("run");
            break;

        case UNIT_ATTACKING_OUT_OF_RANGE:
            object->setAnimation("wave");
            break;

        case UNIT_ATTACKING:
            object->setAnimation("attack");
            break;

         case UNIT_DYING:
            object->setAnimation("death_fallback");
            break;
   }
}

void Unit::setTargetUnit(Unit* target)
{
    if(targetUnit)
        targetUnit->release();

    targetUnit = target;
    targetUnit->retain();

    setUnitState(UNIT_ATTACKING_OUT_OF_RANGE);
}

void Unit::setTargetPosition(vec2 target)
{
    targetPosition = target;
    setUnitState(UNIT_RUNNING);
}

void Unit::receiveDamage(int dmg, Unit* attacker)
{
    if(unitState == UNIT_IDLE) 
    {
        setUnitState(UNIT_ATTACKING_OUT_OF_RANGE);
        targetUnit = attacker;
    }

    health -= dmg;

    if(!isAlive())
        setUnitState(UNIT_DYING);
}
