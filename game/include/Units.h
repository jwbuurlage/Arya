#pragma once 

#include "Arya.h"

using Arya::Object;

typedef struct
{
    float radius;
} UnitInfo;

typedef enum
{
    UNIT_IDLE,
    UNIT_RUNNING,
    UNIT_ATTACKING_OUT_OF_RANGE,
    UNIT_ATTACKING,
    UNIT_DYING
} UnitState;

class Unit
{
    public:
        Unit(UnitInfo* inf);
        ~Unit();

        void setObject(Object* obj);
        Object* getObject() const { return object; }

        UnitInfo* getInfo() const { return info; }
        void setInfo(UnitInfo* unitInfo) { info = unitInfo; }

        void setSelected(bool sel) { selected = sel; }
        bool isSelected() { return selected; }

        void update(float elapsedTime);

        void setTargetPosition(vec2 target);
        void setTargetUnit(Unit* unit);

        void setUnitState(UnitState state);
        UnitState getUnitState() const { return unitState; }

        void receiveDamage(int dmg, Unit* attacker);
        bool isAlive() const { return (health > 0); }
        bool obsolete() { return !isAlive() && (dyingTime > 0.8f); }
        bool readyToDelete() { return refCount <= 0; }

        void retain() { ++refCount; LOG_INFO("now: " << refCount); }
        void release() { --refCount; LOG_INFO("now: " << refCount); }

    private:
        Object* object;
        UnitInfo* info;
        bool selected;

        // movement and attack
        vec2 targetPosition;
        Unit* targetUnit;
        UnitState unitState;

        float timeSinceLastAttack;
        float dyingTime;

        //TODO: move these to UnitInfo
        float speed;
        float yawspeed; //in degrees
        int health;
        int damage;
        float attackSpeed; //the time one attack takes

        int refCount;

};
