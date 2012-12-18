#include "../include/Units.h"
#include "../include/Packet.h"
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
    unitState = UNIT_IDLE;
    targetUnit = 0;

    health = info->maxHealth;
    timeSinceLastAttack = info->attackSpeed + 1.0f;
    dyingTime = 0.0f;

    refCount = 0;

    screenPosition = vec2(0.0);
    tintColor = vec3(0.5);

    // init and register health bar
    healthBar = new Rect;
    healthBar->fillColor = vec4(tintColor, 1.0);
    healthBar->sizeInPixels = vec2(25.0, 3.0);
    // need to check if this flips orientation
    healthBar->offsetInPixels = vec2(-12.5, 25.0);

    // Root::shared().getOverlay()->addRect(healthBar);

    id = -1;
    factionId = -1;
}

Unit::~Unit()
{
    if(targetUnit)
        targetUnit->release();
    if(object) object->setObsolete();

    Root::shared().getOverlay()->removeRect(healthBar);
    delete healthBar;
}

void Unit::setObject(Object* obj)
{
    object = obj;
}

void Unit::update(float timeElapsed)
{
    healthBar->relative = screenPosition;

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

        if(glm::distance(getPosition(), targetUnit->getPosition())
                < targetUnit->getInfo()->radius + info->attackRadius) {
            if(unitState != UNIT_ATTACKING)
                setUnitState(UNIT_ATTACKING);

            if(timeSinceLastAttack > info->attackSpeed)
            {
                // make one attack
                targetUnit->receiveDamage(info->damage, this);
                if(!(targetUnit->isAlive()))
                {
                    targetUnit->release();
                    targetUnit = 0;
                    setUnitState(UNIT_IDLE);
                    timeSinceLastAttack = info->attackSpeed + 1.0f;
                    return;
                }
                timeSinceLastAttack = 0.0f;
            }
            else
                timeSinceLastAttack += timeElapsed;
        }
        else {
            if(unitState != UNIT_ATTACKING_OUT_OF_RANGE)
                setUnitState(UNIT_ATTACKING_OUT_OF_RANGE);
            targetPosition = vec2(targetUnit->getPosition().x,
                            targetUnit->getPosition().z);
        }
    }

    float targeth;
    targeth = Root::shared().getScene()->getMap()->getTerrain()->heightAtGroundPosition(targetPosition.x, targetPosition.y);
    vec3 target(targetPosition.x, targeth, targetPosition.y);
    vec3 diff = target - getPosition();

    if(glm::length(diff) < 2.0) // arbitrary closeness...
    {
        setPosition(target);
        targetPosition = vec2(0.0);
        setUnitState(UNIT_IDLE);
        return;
    }

    if(!object) return;

    float newYaw = (180.0f/M_PI)*atan2(-diff.x, -diff.z);
    float oldYaw = object->getYaw();
    float yawDiff = newYaw - oldYaw;

    if(yawDiff > 180.0f) yawDiff -= 360.0f;
    else if(yawDiff < -180.0f) yawDiff += 360.0f;

    float deltaYaw = timeElapsed * info->yawSpeed + 1.0f;
    if((yawDiff >= 0 && yawDiff < deltaYaw) || (yawDiff <= 0 && yawDiff > -deltaYaw))
    {
        //angle is small enough (less than 1 degree) so we can start walking now
        object->setYaw(newYaw);
        if(unitState == UNIT_ATTACKING)
            return;

        diff = glm::normalize(diff);
        vec3 newPosition = getPosition() + timeElapsed * (info->speed * diff);
        newPosition.y = Root::shared().getScene()->getMap()->getTerrain()->heightAtGroundPosition(newPosition.x, newPosition.z);
        setPosition(newPosition);
    }
    else
    {
        //Rotate
        if(yawDiff < 0) deltaYaw = -deltaYaw;
        object->setYaw( oldYaw + deltaYaw );
    }

}

void Unit::setUnitState(UnitState state)
{
    if(unitState == UNIT_DYING)
        return;

    unitState = state;

    if(!object) return; //server

    switch(unitState)
    {
        case UNIT_IDLE:
            object->setAnimation("stand");
            break;

        case UNIT_RUNNING:
            object->setAnimation("run");
            break;

        case UNIT_ATTACKING_OUT_OF_RANGE:
            object->setAnimation("crouch_walk");
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
    if(targetUnit)
        targetUnit->release();
    targetUnit = 0;

    targetPosition = target;
    setUnitState(UNIT_RUNNING);
}

void Unit::receiveDamage(int dmg, Unit* attacker)
{
    if(unitState == UNIT_IDLE) 
    {
        setUnitState(UNIT_ATTACKING_OUT_OF_RANGE);
        targetUnit = attacker;
        attacker->retain();
    }

    health -= dmg;
    if(health < 0) health = 0;

    healthBar->sizeInPixels = vec2(25.0*getHealthRatio(), 3.0);

    if(!isAlive())
        setUnitState(UNIT_DYING);
}

void Unit::setTintColor(vec3 tC)
{
    tintColor = tC;
    if(object) object->setTintColor(tC);
    healthBar->fillColor = vec4(tintColor, 1.0);
}

void Unit::serialize(Packet& pk)
{
    pk << id;
    pk << factionId;
}

void Unit::deserialize(Packet& pk)
{
    pk >> id;
    pk >> factionId;
}
