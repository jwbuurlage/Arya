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

typedef enum
{
    STANCE_AGGRESSIVE,
    STANCE_DEFENSIVE,
    STANCE_PASSIVE
} UnitStance;

class Packet;

class Unit;

class Map;
class ServerGameSession;

struct CellList;
struct Cell;

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

        void setPosition(const vec3& pos);
        vec3 getPosition() const { return position; }
		vec2 getPosition2() const { return vec2(position.x, position.z); }

        //call setCell with zero to stop using cells
        void setCell(Cell* newCell);
        void setCellFromList(CellList* cl);
        Cell* getCell() const { return currentCell; }
        void checkForEnemies();

        void setYaw(float y){ yaw = y; if(object) object->setYaw(y); }
        float getYaw() const { return yaw; }

        void setObject(Object* obj);
        Object* getObject() const { return object; }

        int getType() const { return type; }
        void setType(int _type) { type = _type; }

        void setSelected(bool sel) { selected = sel; }
        bool isSelected() { return selected; }

        void update(float timeElapsed, Map* map, ServerGameSession* serverSession = 0);

        vec2 getTargetPosition() const { return targetPosition; }
        void setTargetPosition(vec2 target);
        Unit* getTargetUnit() const { return targetUnit; }
        void setTargetUnit(Unit* unit);

        void setUnitState(UnitState state);
        UnitState getUnitState() const { return unitState; }

        void receiveDamage(float dmg, Unit* attacker);
        float getHealthRatio() const { return health / infoForUnitType[type].maxHealth; }

        bool isAlive() const { return (health > 0); }
		void makeDead(){ health = 0; setUnitState(UNIT_DYING); dyingTime = 0.0f; }; //will show death animation and then delete unit
        bool obsolete() { return !isAlive() && (dyingTime > 0.8f); }

        //if this returns true the unit will be deleted by session
        bool readyToDelete() { return refCount <= 0 && !isAlive() && (dyingTime > 0.8f); }
        void markForDelete(){ health = 0; dyingTime = 1.0f; }; //unit will be deleted as soon as refcount reaches zero

        void retain() { ++refCount; }
        void release() { --refCount; }

        void setScreenPosition(vec2 sPos) { screenPosition = sPos; }
        vec2 getScreenPosition() const { return screenPosition; }
        void setTintColor(vec3 tC);

        int getId() const { return id; }

        float getRadius() const { return infoForUnitType[type].radius; }

        void serialize(Packet& pk);
        void deserialize(Packet& pk);

    private:
        Object* object; //object->position and object->yaw are always the same as Unit::position and Unit::yaw
        vec3 position; //since server has no Object, position is stored here
        float yaw;

        int type;
        const int id;
        int factionId;

        // movement and attack
        vec2 targetPosition;
        Unit* targetUnit;
        UnitState unitState;
        Cell* currentCell;
        bool selected;

        vec2 screenPosition;

        float health;
        float timeSinceLastAttackRequest; //to prevent spamming the server
        float timeSinceLastAttack;
        float dyingTime;

        int refCount;
        Rect* healthBar;

        vec3 tintColor;

};
