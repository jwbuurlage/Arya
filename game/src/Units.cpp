#include "../include/common/GameLogger.h"
#include "../include/Units.h"
#include "../include/Packet.h"
#include "../include/Map.h"
#include "../include/Game.h"
#include "../include/EventCodes.h"
#include "../include/GameSession.h"
#include "../include/ServerGameSession.h"
#include "../include/common/Cells.h"
#include <math.h>

#ifdef _WIN32
#define M_PI 3.14159265358979323846
#endif

using Arya::Root;

Unit::Unit(int _type, int _id, GameSession* _session) : session(_session), id(_id)
{
	selectionDecal = 0;
	unitVisionary = 0;

    factionId = -1;
    local = false;
    obsolete = false;
    refCount = 0;
    setType(_type);

    customData = 0;
    createScriptData();

    object = 0;
    position = vec3(0.0f);
    position2 = vec2(0.0f);
    yaw = 0.0f;

    selected = false;
    targetPosition = vec2(0.0f);
    unitState = UNIT_IDLE;
    targetUnit = 0;
    currentCell = 0;

    health = unitInfo->maxHealth;
    timeSinceLastAttack = unitInfo->attackSpeed + 1.0f;
    timeSinceLastAttackRequest = 2.0f;

    dyingTime = 0.0f;

    screenPosition = vec2(0.0);
    tintColor = vec3(0.5);

    ////init and register health bar
    //healthBar = new Rect;
    //healthBar->fillColor = vec4(tintColor, 1.0);
    //healthBar->sizeInPixels = vec2(25.0, 3.0);
    //// need to check if this flips orientation
    //healthBar->offsetInPixels = vec2(-12.5, 25.0);
    //Root::shared().getOverlay()->addRect(healthBar);

    //Register at Game session unit id map
}

Unit::~Unit()
{
    if(targetUnit)
        targetUnit->release();
    if(object) object->setObsolete();

	if(selectionDecal)
		delete selectionDecal;

	if(unitVisionary)
	{
		Root::shared().getScene()->getFogMap()->removeVisionary(unitVisionary);
		delete unitVisionary;
	}

    deleteScriptData();

    session->destroyUnit(id);

    setCell(0);

    //NOT IN SERVER:
    //Root::shared().getOverlay()->removeRect(healthBar);
    //delete healthBar;
}

void Unit::setObject(Object* obj)
{
    object = obj;
}

void Unit::setSelected(bool _sel)
{
	if(!(selected == _sel))
	{
		if(selectionDecal)
		{
			if(_sel)
				Arya::Decals::shared().addDecal(selectionDecal);
			else
				Arya::Decals::shared().removeDecal(selectionDecal);
		}
	}

	selected = _sel;
}

void Unit::updateGraphics()
{
    if(!local || session->isServer()) return;

	if(!selectionDecal)
	{
		selectionDecal = new Decal(Arya::TextureManager::shared().getTexture("selection.png"),
					vec2(0.0, 0.0),
					unitInfo->radius,
					vec3(0.5) );
	}
	else
	{
		selectionDecal->scale = unitInfo->radius;
        selectionDecal->color = tintColor;
	}

	if(!unitVisionary)
	{
		unitVisionary = new Visionary(&position2, &unitInfo->viewRadius);
		Root::shared().getScene()->getFogMap()->addVisionary(unitVisionary);
	}
}

void Unit::setType(int _type)
{
    type = _type;
    unitInfo = getUnitInfo(_type);
    if(unitInfo == 0)
        GAME_LOG_ERROR("UnitInfo for type " << type << " not found! This will crash");
	else
		updateGraphics();
}

void Unit::checkForEnemies()
{
#ifndef SERVERONLY
    if(unitState != UNIT_IDLE)
        return;

    //TODO: Instead of trying to stop the spam with a timer
    //we should actually check if this specific request has been
    //answered or not
    if(timeSinceLastAttackRequest < 0.3f) return;

    if(!currentCell) return;

    CellList* list = currentCell->cellList;
    if(!list) return;

    int curx = currentCell->cellx;
    int cury = currentCell->celly;

    Cell* c;
    float closestDistance = unitInfo->viewRadius;
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
                Unit* unit = session->getUnitById(c->cellPoints[i]);
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
	position2 = vec2(pos.x, pos.z);
    if(object) object->setPosition(pos);
	if(selectionDecal)
		selectionDecal->setPos(getPosition2());

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

void Unit::update(float timeElapsed)
{
    //For any units referenced by this unit we must check if they are obsolete
    //Currently the only referenced unit is targetUnit
    if(targetUnit && !targetUnit->isAlive())
    {
        targetUnit->release();
        targetUnit = 0;
        if(unitState == UNIT_ATTACKING || unitState == UNIT_ATTACKING_OUT_OF_RANGE)
            setUnitState(UNIT_IDLE);
    }

    timeSinceLastAttackRequest += timeElapsed;
    timeSinceLastAttack += timeElapsed;

    if(unitState == UNIT_IDLE)
        return;

    if(unitState == UNIT_DYING)
    {
        dyingTime += timeElapsed;
        //TODO: Use death animation time here
        if(dyingTime > 0.8f) markForDelete();
        return;
    }

    if(obsolete)
    {
        GAME_LOG_DEBUG("Unit " << id << " is obsolete but has state " << unitState);
        setUnitState(UNIT_DYING);
        return;
    }

    //If we are attacking, the target position is the position of the target unit
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
            < targetUnit->getInfo()->radius + unitInfo->attackRadius;
        bool inAngleRange = yawDiff > -20.0f && yawDiff < 20.0f;

        if(inRange && inAngleRange)
        {
            if(!unitInfo->canMoveWhileAttacking)
                canMove = false;

            if(unitState != UNIT_ATTACKING)
            {
                //If the time since last attack is LESS we do not want to reset
                //it because then the unit could attack much faster by going
                //in-out-in-out of range. When the time is MORE then we want to
                //cap it because otherwise it could build up attacks
                if(timeSinceLastAttack > unitInfo->attackSpeed)
                    timeSinceLastAttack = unitInfo->attackSpeed;
                setUnitState(UNIT_ATTACKING);
            }

            while(timeSinceLastAttack >= unitInfo->attackSpeed)
            {
                timeSinceLastAttack -= unitInfo->attackSpeed;

                targetUnit->receiveDamage(unitInfo->damage, this);

                //When the unit dies the server sends a packet
                //The client leaves the unit alive untill it receives the packet
                if(session->isServer())
                {
                    //TODO: just call session->onUnitDied() which handles this!!
                    ServerGameSession* serverSession = (ServerGameSession*)session;
                    if(!targetUnit->isAlive())
                    {
                        targetUnit->getInfo()->onDeath(targetUnit);
                        //Note that the unit was alive before this damage so this must have killed it
                        //Therefore we can send the death packet here
                        Packet* pak = serverSession->createPacket(EVENT_UNIT_DIED);
                        *pak << targetUnit->id;
                        serverSession->sendToAllClients(pak);
                        targetUnit->markForDelete();
                        targetUnit->release();
                        targetUnit = 0;
                        setUnitState(UNIT_IDLE);
                        return;
                    }
                }
            }
        }
        else
        {
            if(unitState != UNIT_ATTACKING_OUT_OF_RANGE)
                setUnitState(UNIT_ATTACKING_OUT_OF_RANGE);
        }
    }

    float deltaYaw = timeElapsed * unitInfo->yawSpeed;
    //check if we reached target angle
    if( abs(yawDiff) < deltaYaw )
    {
        //Target angle reached
        setYaw(newYaw);

        //A part of the current frametime went into rotating
        //we must now calculate how many frametime is left for moving
        float remainingTime = (deltaYaw - abs(yawDiff)) / unitInfo->yawSpeed;

        if(canMove)
        {
            //When we are close to the target, we might go past the target because
            //of high speed or high frametime so we have to check for this
            float distanceToTravel = remainingTime * unitInfo->speed;

            vec2 newPosition;
            if( distanceToTravel >= difflength )
            {
                newPosition = targetPosition;
                setUnitState(UNIT_IDLE);
            }
            else
                newPosition = getPosition2() + distanceToTravel * glm::normalize(diff);

            Map* map = session->getMap();
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
    if(unitState == UNIT_DYING)
        return;

    unitState = state;

#ifndef SERVERONLY
    if(!object) return; //server

    switch(unitState)
    {
        case UNIT_IDLE:
            setTintColor(vec3(0.0,0.0,0.0));
            object->setAnimation("stand");
            break;

        case UNIT_RUNNING:
            setTintColor(vec3(0.0,1.0,0.0));
            object->setAnimation("run");
            break;

        case UNIT_ATTACKING_OUT_OF_RANGE:
            setTintColor(vec3(0.0,0.0,1.0));
            object->setAnimation("crouch_walk");
            break;

        case UNIT_ATTACKING:
            setTintColor(vec3(1.0,0.0,0.0));
            object->setAnimation("attack");
            break;

        case UNIT_DYING:
            setTintColor(vec3(1.0,1.0,1.0));
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

    if(unitState == UNIT_DYING)
        GAME_LOG_DEBUG("Unit " << id << " probable error at setTargetUnit");

    setUnitState(UNIT_ATTACKING_OUT_OF_RANGE);
}

void Unit::setTargetPosition(vec2 target)
{
    if(targetUnit)
        targetUnit->release();
    targetUnit = 0;

    if(unitState == UNIT_DYING)
        GAME_LOG_DEBUG("Unit " << id << " probable error at setTargetPosition");

    targetPosition = target;
    setUnitState(UNIT_RUNNING);
}

void Unit::receiveDamage(float dmg, Unit* attacker)
{
    if(local)
    {
        if(timeSinceLastAttackRequest > 1.0f)
        {
            if(unitState == UNIT_IDLE) //and if not in passive mode
            {
				timeSinceLastAttackRequest = 0;
                Event& ev = Game::shared().getEventManager()->createEvent(EVENT_ATTACK_MOVE_UNIT_REQUEST);
                ev << 1;
                ev << id << attacker->getId();
                ev.send();
            }
        }
    }

    health -= dmg;
    if(health < 0) health = 0;

    unitInfo->onDamage(this, attacker, dmg);

    //healthBar->sizeInPixels = vec2(25.0*getHealthRatio(), 3.0);

    //This is done when the death packet is received
    //if(!isAlive())
    //{
    //    setUnitState(UNIT_DYING);
    //}
}

void Unit::setTintColor(vec3 tC)
{
    tintColor = tC;
    if(object) object->setTintColor(tC);
    if(selectionDecal) selectionDecal->color = tC;
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
    int _type;
    pk >> _type;
    setType(_type);
    pk >> factionId;
    pk >> position;
    int _unitState;
    pk >> _unitState;
    unitState = (UnitState)_unitState;
    pk >> targetPosition;
    int targetUnitId;
    pk >> targetUnitId;
    if(targetUnitId) targetUnit = session->getUnitById(targetUnitId);
}
void Unit::getDebugText()
{
	GAME_LOG_DEBUG("Unit id = " << id);
	GAME_LOG_DEBUG("Unit factionId = " << factionId);
	GAME_LOG_DEBUG("Unit obsolete = " << obsolete);
	GAME_LOG_DEBUG("Unit refCount = " << refCount);
	GAME_LOG_DEBUG("Unit health = " << health);
	GAME_LOG_DEBUG("Unit dyingTime = " << dyingTime);
	GAME_LOG_DEBUG("targetUnit = " << (targetUnit ? targetUnit->getId() : 0));
	GAME_LOG_DEBUG("Unit state = " << unitState);
	GAME_LOG_DEBUG("------------------------------------");
}
