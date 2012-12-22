#pragma once 

#include "Arya.h"
#include "UnitTypes.h"

#include <map>
using std::map;
using std::pair;

using Arya::Object;
using Arya::Rect;
using Arya::Root;

typedef enum
{
    UNIT_IDLE,
    UNIT_RUNNING,
    UNIT_ATTACKING_OUT_OF_RANGE,
    UNIT_ATTACKING,
    UNIT_DYING
} UnitState;

class Packet;

class Unit;

//Factory design pattern
class UnitFactory
{
    public:
        UnitFactory(){}
        virtual ~UnitFactory(){}

        //destory units by calling delete on them
        Unit* createUnit(int id, int type);
        Unit* getUnitById(int id);
    private:
        map<int,Unit*> unitMap;
        typedef map<int,Unit*>::iterator unitMapIterator;

        friend class Unit;
        void destroyUnit(int id);
};

class Unit
{
    private:
        friend class UnitFactory;
        UnitFactory* unitFactory;

        Unit(int _type, int id, UnitFactory* factory);
    public:
        ~Unit(); //unregisters itself at unit factory

        void setPosition(const vec3& pos) { position = pos; if(object) object->setPosition(pos); }
        vec3 getPosition() const { return position; }

        void setObject(Object* obj);
        Object* getObject() const { return object; }

        int getType() const { return type; }
        void setType(int _type) { type = _type; }

        void setSelected(bool sel) { selected = sel; }
        bool isSelected() { return selected; }

        void update(float elapsedTime);

        vec2 getTargetPosition() const { return targetPosition; }
        void setTargetPosition(vec2 target);
        Unit* getTargetUnit() const { return targetUnit; }
        void setTargetUnit(Unit* unit);

        void setUnitState(UnitState state);
        UnitState getUnitState() const { return unitState; }

        void receiveDamage(float dmg, Unit* attacker);
        float getHealthRatio() const { return health / infoForUnitType[type].maxHealth; }

        bool isAlive() const { return (health > 0); }
        bool obsolete() { return !isAlive() && (dyingTime > 0.8f); }
        bool readyToDelete() { return refCount <= 0; }

        void retain() { ++refCount; }
        void release() { --refCount; }

        void setScreenPosition(vec2 sPos) { screenPosition = sPos; }
        vec2 getScreenPosition() const { return screenPosition; }
        void setTintColor(vec3 tC);

        int getId() const { return id; }

        void serialize(Packet& pk);
        void deserialize(Packet& pk);

    private:
        Object* object; //object->position is always the same as position
        vec3 position; //since server has no Object, position is stored here
        int type;
        const int id;
        int factionId;

        // movement and attack
        vec2 targetPosition;
        Unit* targetUnit;
        UnitState unitState;
        bool selected;

        vec2 screenPosition;

        float health;
        float timeSinceLastAttack;
        float dyingTime;

        int refCount;
        Rect* healthBar;

        vec3 tintColor;

};
