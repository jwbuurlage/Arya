#include "../include/common/GameLogger.h"
#include "../include/Units.h"
#include "../include/Packet.h"
#include "../include/Map.h"
#include "../include/EventCodes.h"
#include "../include/ServerGameSession.h"
#include <math.h>

#ifdef _WIN32
#define M_PI 3.14159265358979323846
#endif

using Arya::Root;

Unit* UnitFactory::createUnit(int id, int type)
{
    Unit* unit = getUnitById(id);
    if(unit)
    {
        GAME_LOG_WARNING("Trying to create unit with duplicate id (" << id << ")");
        return unit;
    }
    unit = new Unit(type, id, this);
    unitMap.insert(pair<int,Unit*>(unit->getId(),unit));
    return unit;
}

//Called from Unit deconstructor
void UnitFactory::destroyUnit(int id)
{
    unitMapIterator iter = unitMap.find(id);
    if(iter == unitMap.end())
    {
        GAME_LOG_WARNING("Trying to destory unexisting unit id");
        return;
    }
    unitMap.erase(iter);
    return;
}

Unit* UnitFactory::getUnitById(int id)
{
    unitMapIterator iter = unitMap.find(id);
    if(iter == unitMap.end()) return 0;
    return iter->second;
}

Unit::Unit(int _type, int _id, UnitFactory* factory) : id(_id)
{
    type = _type;
    unitFactory = factory;

    object = 0;
    position = vec3(0.0f);
    yaw = 0.0f;

    selected = false;
    targetPosition = vec2(0.0f);
    unitState = UNIT_IDLE;
    targetUnit = 0;

    health = infoForUnitType[type].maxHealth;
    timeSinceLastAttack = infoForUnitType[type].attackSpeed + 1.0f;
    dyingTime = 0.0f;

    refCount = 0;

    screenPosition = vec2(0.0);
    tintColor = vec3(0.5);

    ////init and register health bar
    //healthBar = new Rect;
    //healthBar->fillColor = vec4(tintColor, 1.0);
    //healthBar->sizeInPixels = vec2(25.0, 3.0);
    //// need to check if this flips orientation
    //healthBar->offsetInPixels = vec2(-12.5, 25.0);
    //Root::shared().getOverlay()->addRect(healthBar);

    factionId = -1;

    //Register at Game session unit id map
}

Unit::~Unit()
{
    if(targetUnit)
        targetUnit->release();
    if(object) object->setObsolete();

    unitFactory->destroyUnit(id);

    //NOT IN SERVER:
    //Root::shared().getOverlay()->removeRect(healthBar);
    //delete healthBar;
}

void Unit::setObject(Object* obj)
{
    object = obj;
}

void Unit::checkForEnemies(QuadTree* qt)
{
    if(unitState != UNIT_IDLE)
        return;

    // shoot the damn guy
    int cId = qt->closestId(getPosition2());
    if(cId > 0)
    {
        Unit* closestUnit = unitFactory->getUnitById(cId);
        if(glm::distance(getPosition(), closestUnit->getPosition()) < infoForUnitType[type].attackRadius)
            setTargetUnit(closestUnit);
    }
}

void Unit::update(float timeElapsed, Map* map)
{
    //healthBar->relative = screenPosition;

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
            GAME_LOG_WARNING("Attacking, but no target unit");
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

        if(glm::distance(getPosition2(), targetUnit->getPosition2())
                < infoForUnitType[targetUnit->getType()].radius + infoForUnitType[type].attackRadius) {
            if(unitState != UNIT_ATTACKING)
                setUnitState(UNIT_ATTACKING);

            if(timeSinceLastAttack > infoForUnitType[type].attackSpeed)
            {
                // make one attack
                targetUnit->receiveDamage(infoForUnitType[type].damage, this);
                if(!(targetUnit->isAlive()))
                {
                    targetUnit->release();
                    targetUnit = 0;
                    setUnitState(UNIT_IDLE);
                    timeSinceLastAttack = infoForUnitType[type].attackSpeed + 1.0f;
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

    if(!map) return;

    float targeth;
    targeth = map->heightAtGroundPosition(targetPosition.x, targetPosition.y);
    vec3 target(targetPosition.x, targeth, targetPosition.y);
    vec3 diff = target - getPosition();

    if(glm::length(diff) < 2.0) // arbitrary closeness...
    {
        setPosition(target);
        targetPosition = vec2(0.0);
        setUnitState(UNIT_IDLE);
        return;
    }

    float newYaw = (180.0f/M_PI)*atan2(-diff.x, -diff.z);
    float oldYaw = getYaw();
    float yawDiff = newYaw - oldYaw;

    if(yawDiff > 180.0f) yawDiff -= 360.0f;
    else if(yawDiff < -180.0f) yawDiff += 360.0f;

    float deltaYaw = timeElapsed * infoForUnitType[type].yawSpeed + 1.0f;
    if((yawDiff >= 0 && yawDiff < deltaYaw) || (yawDiff <= 0 && yawDiff > -deltaYaw))
    {
        //angle is small enough (less than 1 degree) so we can start walking now
        setYaw(newYaw);
        if(unitState == UNIT_ATTACKING)
            return;

        diff = glm::normalize(diff);
        vec3 newPosition = getPosition() + timeElapsed * (infoForUnitType[type].speed * diff);
        newPosition.y = map->heightAtGroundPosition(newPosition.x, newPosition.z);
        setPosition(newPosition);
    }
    else
    {
        //Rotate
        if(yawDiff < 0) deltaYaw = -deltaYaw;
        setYaw( oldYaw + deltaYaw );
    }

}

void Unit::serverUpdate(float timeElapsed, Map* map, ServerGameSession* serverSession)
{
    if(unitState == UNIT_IDLE)
        return;

    if(unitState == UNIT_ATTACKING || 
            unitState == UNIT_ATTACKING_OUT_OF_RANGE)
    {
        if(!targetUnit) {
            GAME_LOG_WARNING("Unit (" << id << ") is attacking, but no target unit");
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

		//Unit is alive here

        if(glm::distance(getPosition2(), targetUnit->getPosition2())
                < infoForUnitType[targetUnit->getType()].radius + infoForUnitType[type].attackRadius) {
            if(unitState != UNIT_ATTACKING)
                setUnitState(UNIT_ATTACKING);

            if(timeSinceLastAttack > infoForUnitType[type].attackSpeed)
            {
                // make one attack
                targetUnit->receiveDamage(infoForUnitType[type].damage, this);
                if(!(targetUnit->isAlive()))
                {
					//Note that the unit was alive before this damage so this must have killed it
					//Therefore we can send the death packet here
					Packet* pak = serverSession->createPacket(EVENT_UNIT_DIED);
					*pak << targetUnit->id;
					serverSession->sendToAllClients(pak);
					//Kill the unit
					targetUnit->makeObsolete();
                    targetUnit->release();
                    targetUnit = 0;

                    setUnitState(UNIT_IDLE);
                    timeSinceLastAttack = infoForUnitType[type].attackSpeed + 1.0f;
                    return;
                }
                timeSinceLastAttack -= infoForUnitType[type].attackSpeed;
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

    if(!map) return;

    float targeth;
    targeth = map->heightAtGroundPosition(targetPosition.x, targetPosition.y);
    vec3 target(targetPosition.x, targeth, targetPosition.y);
    vec3 diff = target - getPosition();

    if(glm::length(diff) < 2.0) // arbitrary closeness...
    {
        setPosition(target);
        targetPosition = vec2(0.0);
        setUnitState(UNIT_IDLE);
        return;
    }

    float newYaw = (180.0f/M_PI)*atan2(-diff.x, -diff.z);
    float oldYaw = getYaw();
    float yawDiff = newYaw - oldYaw;

    if(yawDiff > 180.0f) yawDiff -= 360.0f;
    else if(yawDiff < -180.0f) yawDiff += 360.0f;

    float deltaYaw = timeElapsed * infoForUnitType[type].yawSpeed + 1.0f;
    if((yawDiff >= 0 && yawDiff < deltaYaw) || (yawDiff <= 0 && yawDiff > -deltaYaw))
    {
        //angle is small enough (less than 1 degree) so we can start walking now
        setYaw(newYaw);
        if(unitState == UNIT_ATTACKING)
            return;

        diff = glm::normalize(diff);
        vec3 newPosition = getPosition() + timeElapsed * (infoForUnitType[type].speed * diff);
        newPosition.y = map->heightAtGroundPosition(newPosition.x, newPosition.z);
        setPosition(newPosition);
    }
    else
    {
        //Rotate
        if(yawDiff < 0) deltaYaw = -deltaYaw;
        setYaw( oldYaw + deltaYaw );
    }
}

void Unit::setUnitState(UnitState state)
{
    //if(unitState == UNIT_DYING)
    //    return;

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

void Unit::receiveDamage(float dmg, Unit* attacker)
{
    if(unitState == UNIT_IDLE) 
    {
        setUnitState(UNIT_ATTACKING_OUT_OF_RANGE);
        targetUnit = attacker;
        attacker->retain();
    }

    health -= dmg;
    if(health < 0) health = 0;

    //healthBar->sizeInPixels = vec2(25.0*getHealthRatio(), 3.0);

    if(!isAlive())
        setUnitState(UNIT_DYING);
}

void Unit::setTintColor(vec3 tC)
{
    tintColor = tC;
    if(object) object->setTintColor(tC);
    //healthBar->fillColor = vec4(tintColor, 1.0);
}

void Unit::serialize(Packet& pk)
{
    pk << type;
    pk << factionId;
    pk << position;
	pk << (int)unitState;
	pk << targetPosition;
	pk << (targetUnit ? targetUnit->getId() : 0);
}

void Unit::deserialize(Packet& pk)
{
    pk >> type;
    pk >> factionId;
    pk >> position;
	pk >> (int&)unitState;
	pk >> targetPosition;
	int targetUnitId;
	pk >> targetUnitId;
	if(targetUnitId) targetUnit = unitFactory->getUnitById(targetUnitId);
}
