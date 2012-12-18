#pragma once 

#include "Arya.h"

using Arya::Object;
using Arya::Rect;
using Arya::Root;

typedef struct
{
    float radius;
    float attackRadius;
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

class Packet;

class Unit
{
    public:
        Unit(UnitInfo* inf);
        ~Unit();

        void setPosition(const vec3& pos) { position = pos; if(object) object->setPosition(pos); }
        vec3 getPosition() const { return position; }

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
        void setTintColor(vec3 tC);

        void serialize(Packet& pk);
        void deserialize(Packet& pk);

    private:
        Object* object; //object->position is always the same as position
        vec3 position; //since server has no Object, position is stored here
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

        int id;
        int factionId;
};
