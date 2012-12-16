#pragma once 

#include "Arya.h"

using Arya::Object;
using Arya::Rect;
using Arya::Root;

typedef struct
{
    float radius;
    float speed;
    float yawSpeed; //in degrees

    float maxHealth;
    float damage;
    float attackSpeed; //the time one attack takes
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
        float getHealthRatio() const { return health / info->maxHealth; }

        bool isAlive() const { return (health > 0); }
        bool obsolete() { return !isAlive() && (dyingTime > 0.8f); }
        bool readyToDelete() { return refCount <= 0; }

        void retain() { ++refCount; }
        void release() { --refCount; }

        void setScreenPosition(vec2 sPos) { screenPosition = sPos; }
        vec2 getScreenPosition() const { return screenPosition; }
        vec3 setTintColor(vec3 tC);

    private:
        Object* object;
        UnitInfo* info;
        bool selected;

        // movement and attack
        vec2 targetPosition;
        Unit* targetUnit;
        UnitState unitState;

        vec2 screenPosition;

        float health;
        float timeSinceLastAttack;
        float dyingTime;

        int refCount;
        Rect* healthBar;

        vec3 tintColor;
};
