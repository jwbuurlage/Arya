#pragma once 

#include "Arya.h"
#include "UnitTypes.h"

using Arya::Object;
using Arya::Rect;
using Arya::Decal;
using Arya::Root;
using Arya::Visionary;

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
class GameSession;
class ServerGameSession;
class LuaScriptData;
struct CellList;
struct Cell;

class Unit
{
    private:
        //Only GameSession can create units
        //This ensures that every unit is registered
        //at the session and that every unit has a pointer
        //to the session
        friend class GameSession;
        GameSession* const session;

        Unit(int _type, int id, GameSession* session);
    public:
        ~Unit(); //unregisters itself at session

        void setPosition(const vec3& pos);
        vec3 getPosition() const { return position; }
		vec2 getPosition2() const { return vec2(position.x, position.z); }

        //call setCell with zero to stop using cells
        void setCell(Cell* newCell);
        void setCellFromList(CellList* cl);
        Cell* getCell() const { return currentCell; }
        void checkForEnemies();
		bool checkForCollision(vec2 checkPosition, float checkHeight);

        void setYaw(float y){ yaw = y; if(object) object->setYaw(y); }
        float getYaw() const { return yaw; }

        void setObject(Object* obj);
        Object* getObject() const { return object; }

        int getType() const { return type; }
        void setType(int _type);
        UnitInfo* getInfo() const { return unitInfo; }

        void setSelected(bool sel);
        bool isSelected() { return selected; }

        void update(float timeElapsed);

        vec2 getTargetPosition() const { return targetPosition; }
        void setTargetPosition(vec2 target);
        Unit* getTargetUnit() const { return targetUnit; }
        void setTargetUnit(Unit* unit);
		std::vector<vec2>& getPathNodes(){ return pathNodes; }

        void setUnitState(UnitState state);
        UnitState getUnitState() const { return unitState; }

        void receiveDamage(float dmg, Unit* attacker);
        float getHealth() const { return health; }
        float getHealthRatio() const { return health / unitInfo->maxHealth; }

        bool isAlive() const { return (!obsolete && health > 0); }
		void makeDead(){ health = 0; setUnitState(UNIT_DYING); dyingTime = 0.0f; }; //will show death animation and then delete unit

        //if this returns true the unit will be deleted by session
        bool readyToDelete() const { return obsolete && refCount <= 0; }
        void markForDelete(){ obsolete = true; } //unit will be deleted as soon as refcount reaches zero

        void retain() { ++refCount; }
        void release() { --refCount; }

        void setScreenPosition(vec2 sPos) { screenPosition = sPos; }
        vec2 getScreenPosition() const { return screenPosition; }
        void setTintColor(vec3 tC);

        int getId() const { return id; }
        int getFactionId() const { return factionId; }
        void setFactionId(int id) { factionId = id; }
        void setLocal(bool value = true){ local = value; updateGraphics(); }
        bool isLocal() const { return local; }

        float getRadius() const { return unitInfo->radius; }

        void serialize(Packet& pk);
        void deserialize(Packet& pk);

		void getDebugText();
		void updateGraphics();

        //Must be public because it must be accessed by some
        //scripting functions
        LuaScriptData* customData;
    private:
        Object* object; //object->position and object->yaw are always the same as Unit::position and Unit::yaw
        vec3 position; //since server has no Object, position is stored here
        vec2 position2; // we want to hold this reference for vision
        float yaw;

        UnitInfo* unitInfo;
        int type;
        const int id;
        int factionId;
        bool local; //This must be false on the server. When true the update functions will do auto-attack requests
        bool obsolete;
        int refCount;

        //These functions are implemented in Scripting.cpp and it just calls 'new LuaScriptData' but it needs luabind headers
        //It is called in the Unit constructor, and in the deconstructor we call the delete one
        void createScriptData();
        void deleteScriptData();

        // movement and attack
        vec2 targetPosition;
		std::vector<vec2> pathNodes;
        Unit* targetUnit;
        UnitState unitState;
        Cell* currentCell;
        bool selected;

        float health;
        float timeSinceLastAttackRequest; //to prevent spamming the server
        float timeSinceLastAttack;
        float dyingTime;

        Rect* healthBar;
		Decal* selectionDecal;
		Visionary* unitVisionary;
        vec2 screenPosition;
        vec3 tintColor;
};
