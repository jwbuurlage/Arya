#include "../include/common/GameLogger.h"
#include "../include/Units.h"
#include "../include/Packet.h"
#include "../include/Map.h"
#include "../include/Game.h"
#include "../include/EventCodes.h"
#include "../include/ServerGameSession.h"
#include "../include/common/Cells.h"
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
    currentCell = 0;

    health = infoForUnitType[type].maxHealth;
    timeSinceLastAttack = infoForUnitType[type].attackSpeed + 1.0f;
    timeSinceLastAttackRequest = 2.0f;

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

void Unit::checkForEnemies()
{
#ifndef SERVERONLY
    if(unitState != UNIT_IDLE)
        return;

    if(timeSinceLastAttackRequest < 1.0f) return;

    if(!currentCell) return;

    CellList* list = currentCell->cellList;
    if(!list) return;

    int curx = currentCell->cellx;
    int cury = currentCell->celly;

    Cell* c;
    float closestDistance = infoForUnitType[type].attackRadius * 2.0;
    int closestId = -1;

    float d;
    // loop through neighbours
    for(int dx = -1; dx <= 1; ++dx)
        for(int dy = -1; dy <= 1; ++dy)
        {
            if(curx + dx >= list->gridSize || curx + dx < 0) continue;
            if(cury + dy >= list->gridSize || cury + dy < 0) continue;
            c = list->cellForIndex(curx + dx, cury + dy);
            // loop through
            for(unsigned i = 0; i < c->cellPoints.size(); ++i)
            {
                Unit* unit = unitFactory->getUnitById(c->cellPoints[i]);
                if(!unit) continue;
                if(unit->factionId == factionId) continue;
                d = glm::distance(position, unit->getPosition());

                if(d < closestDistance)
                {
                    closestDistance = d;
                    closestId = c->cellPoints[i];
                }
            }
        }

    if(closestId >= 0)
    {
        timeSinceLastAttackRequest = 0;
        //TODO: Instead of a single event for each unit we can combine
        //all attacks into a single event (currently there is a '1' as count)
        Event& ev = Game::shared().getEventManager()->createEvent(EVENT_ATTACK_MOVE_UNIT_REQUEST);
        ev << 1;
        ev << id << closestId;
        ev.send();
    }
#endif
}

void Unit::setPosition(const vec3& pos)
{
    position = pos;
    if(object) object->setPosition(pos);

    if(currentCell)
    {
        Cell* newCell = currentCell->cellList->cellForPosition(getPosition2());
        if(currentCell != newCell)
        {
            currentCell->remove(id);
            currentCell = newCell;
            currentCell->add(id);
        }
    }
}

void Unit::setCell(Cell* newCell)
{
    if(currentCell != newCell)
    {
        if(currentCell) currentCell->remove(id);
        currentCell = newCell;
        if(currentCell) currentCell->add(id);
    }
}

void Unit::setCellFromList(CellList* cl)
{
    setCell(cl->cellForPosition(getPosition2()));
}

void Unit::update(float timeElapsed, Map* map, ServerGameSession* serverSession)
{
    //For any units referenced by this unit we must check if they are obsolete
    //Currently the only referenced unit is targetUnit
    if(targetUnit && (targetUnit->obsolete() || !targetUnit->isAlive()))
    {
        targetUnit->release();
        targetUnit = 0;
        if(unitState == UNIT_ATTACKING || unitState == UNIT_ATTACKING_OUT_OF_RANGE)
            setUnitState(UNIT_IDLE);
    }

    timeSinceLastAttackRequest += timeElapsed;

    if(unitState == UNIT_IDLE)
        return;

    if(unitState == UNIT_DYING)
    {
        dyingTime += timeElapsed;
        return;
    }

    //If we are attacking the target position is the position of the target unit
    if(unitState == UNIT_ATTACKING || unitState == UNIT_ATTACKING_OUT_OF_RANGE)
    {
        if(!targetUnit)
        {
            GAME_LOG_WARNING("Unit (" << id << ") is attacking, but no target unit");
            setUnitState(UNIT_IDLE);
            return;
        }
        targetPosition = targetUnit->getPosition2();
    }

    // Rotation and movement
    // Even when we are in attacking mode we must still
    // check if we are faced towards the target
    // so these calculations are always done first

    vec2 diff = targetPosition - getPosition2();
    float difflength = glm::length(diff);

    float newYaw = (difflength < 0.1 ? getYaw() : (180.0f/M_PI)*atan2(-diff.x, -diff.y));
    float yawDiff = newYaw - getYaw();
    //make sure it is in the [-180,180] range
    //so that a small rotation is always a small number
    //instead of 359 degrees
    if(yawDiff > 180.0f) yawDiff -= 360.0f;
    else if(yawDiff < -180.0f) yawDiff += 360.0f;

    //At this point we can have a switch or some if statements
    //to decide what to do for each unit state.
    //After this the unit will always be rotated to their target
    //angle (we assume this can be done in any state, even during attacks)
    //If canMove is left to true it will also move when fully rotated
    bool canMove = true;
    if(unitState == UNIT_ATTACKING || unitState == UNIT_ATTACKING_OUT_OF_RANGE)
    {
        bool inRange = glm::distance(getPosition2(), targetUnit->getPosition2())
            < infoForUnitType[targetUnit->getType()].radius + infoForUnitType[type].attackRadius;
        bool inAngleRange = yawDiff > -20.0f && yawDiff < 20.0f;

        if(inRange && inAngleRange)
        {
            if(!infoForUnitType[type].canMoveWhileAttacking)
                canMove = false;

            if(unitState != UNIT_ATTACKING)
                setUnitState(UNIT_ATTACKING);

            timeSinceLastAttack += timeElapsed;

            while(timeSinceLastAttack > infoForUnitType[type].attackSpeed)
            {
                timeSinceLastAttack -= infoForUnitType[type].attackSpeed;

                targetUnit->receiveDamage(infoForUnitType[type].damage, this);
                if(!(targetUnit->isAlive()))
                {
                    if(serverSession)
                    {
                        //Note that the unit was alive before this damage so this must have killed it
                        //Therefore we can send the death packet here
                        Packet* pak = serverSession->createPacket(EVENT_UNIT_DIED);
                        *pak << targetUnit->id;
                        serverSession->sendToAllClients(pak);
                        targetUnit->markForDelete();
                    }
                    targetUnit->release();
                    targetUnit = 0;

                    setUnitState(UNIT_IDLE);
                    timeSinceLastAttack = infoForUnitType[type].attackSpeed + 1.0f;
                    return;
                }
            }
        }
        else
        {
            if(unitState != UNIT_ATTACKING_OUT_OF_RANGE)
                setUnitState(UNIT_ATTACKING_OUT_OF_RANGE);
        }
    }

    float deltaYaw = timeElapsed * infoForUnitType[type].yawSpeed;
    //check if we reached target angle
    if( abs(yawDiff) < deltaYaw )
    {
        //Target angle reached
        setYaw(newYaw);

        //A part of the current frametime went into rotating
        //we must now calculate how many frametime is left for moving
        float remainingTime = (deltaYaw - abs(yawDiff)) / infoForUnitType[type].yawSpeed;

        if(canMove)
        {
            //When we are close to the target, we might go past the target because
            //of high speed or high frametime so we have to check for this
            float distanceToTravel = remainingTime * infoForUnitType[type].speed;

            vec2 newPosition;
            if( distanceToTravel >= difflength )
            {
                newPosition = targetPosition;
                setUnitState(UNIT_IDLE);
            }
            else
                newPosition = getPosition2() + distanceToTravel * glm::normalize(diff);

            float height = (map ? map->heightAtGroundPosition(newPosition.x, newPosition.y) : 0.0f);
            setPosition(vec3(newPosition.x, height, newPosition.y));
        }
    }
    else
    {
        //Target angle not reached, rotate
        if(yawDiff < 0) deltaYaw = -deltaYaw;
        setYaw( getYaw() + deltaYaw );
    }
}

void Unit::setUnitState(UnitState state)
{
    //if(unitState == UNIT_DYING)
    //    return;

    unitState = state;

#ifndef SERVERONLY
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
#endif
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
    {
        setUnitState(UNIT_DYING);
    }
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
