#include "../include/Scripting.h"
#include "../include/UnitTypes.h"
#include "../include/Units.h"
#include "../include/common/GameLogger.h"
#include "../include/Arya.h"
#include <luabind/luabind.hpp>

class LuaUnitType : public UnitInfo
{
    public:
        //registers itself as a unit type
        LuaUnitType(const std::string& _name);
        ~LuaUnitType();

        //These are called by the game when the event occurs
        void onDeath(Unit* unit);
        void onSpawn(Unit* unit);
        void onDamage(int victimId, int attackerId, float damage);

        //These are called by the scripts to set handlers
        void setOnDeath(const luabind::object& obj);
        void setOnSpawn(const luabind::object& obj);
        void setOnDamage(const luabind::object& obj);

    private:
        luabind::object objOnDeath;
        luabind::object objOnSpawn;
        luabind::object objOnDamage;
};


LuaUnitType::LuaUnitType(const std::string& _name) : UnitInfo()
{
    name = _name;
    registerNewUnitInfo(this);
    GAME_LOG_DEBUG("LuaUnitType constructor. New unit type bitches: " << name);
}

LuaUnitType::~LuaUnitType()
{
    GAME_LOG_DEBUG("LuaUnitType deconstructor!");
}

void LuaUnitType::onDeath(Unit* unit)
{
    if(objOnDeath) luabind::call_function<void>(objOnDeath, unit->getId());
}
void LuaUnitType::onSpawn(Unit* unit)
{
    if(objOnSpawn) luabind::call_function<void>(objOnSpawn, unit->getId());
}
void LuaUnitType::onDamage(int victimId, int attackerId, float damage)
{
    if(objOnDamage) luabind::call_function<void>(objOnDamage, victimId, attackerId, damage);
}

void LuaUnitType::setOnDeath(const luabind::object& obj)
{
    if(luabind::type(obj) == LUA_TFUNCTION) objOnDeath = obj;
}
void LuaUnitType::setOnSpawn(const luabind::object& obj)
{
    if(luabind::type(obj) == LUA_TFUNCTION) objOnSpawn = obj;
}
void LuaUnitType::setOnDamage(const luabind::object& obj)
{
    if(luabind::type(obj) == LUA_TFUNCTION) objOnDamage = obj;
}

void luaPrint(const std::string& msg)
{
    GAME_LOG_DEBUG("Script: " << msg);
}

Scripting::Scripting()
{
    luaState = 0;
}

Scripting::~Scripting()
{
    cleanup();
}

int Scripting::init()
{
    if(luaState) cleanup();

    luaState = lua_open();
//    luaopen_base(luaState);
//    luaopen_string(luaState);
//    luaopen_table(luaState);
//    luaopen_math(luaState);
//    luaopen_io(luaState);
//    luaopen_debug(luaState);

    luabind::open(luaState);

    luabind::module(luaState)[
        luabind::def("print", &luaPrint),
        luabind::class_<UnitInfo>("UnitInfoBase"), //should not be used in scripts
        luabind::class_<LuaUnitType, UnitInfo>("UnitType")
            .def(luabind::constructor<const std::string&>())
            .def("setOnDeath", &LuaUnitType::setOnDeath)
            .def("setOnSpawn", &LuaUnitType::setOnSpawn)
            .def("setOnDamage", &LuaUnitType::setOnDamage)
            .def_readonly("name", &LuaUnitType::name)
            .def_readwrite("modelname", &LuaUnitType::modelname)
            .def_readwrite("radius", &LuaUnitType::radius)
            .def_readwrite("attackRadius", &LuaUnitType::attackRadius)
            .def_readwrite("viewRadius", &LuaUnitType::viewRadius)
            .def_readwrite("speed", &LuaUnitType::speed)
            .def_readwrite("yawSpeed", &LuaUnitType::yawSpeed)
            .def_readwrite("maxHealth", &LuaUnitType::maxHealth)
            .def_readwrite("damage", &LuaUnitType::damage)
            .def_readwrite("attackSpeed", &LuaUnitType::attackSpeed)
            .def_readwrite("canMoveWhileAttacking", &LuaUnitType::canMoveWhileAttacking)
            .def_readwrite("selectionSound", &LuaUnitType::selectionSound)
            .def_readwrite("attackSound", &LuaUnitType::attackSound)
        ];

    return 1;
}

void Scripting::cleanup()
{
    if(luaState)
        lua_close(luaState);
    luaState = 0;
}

int Scripting::execute(const char* filename)
{
    Arya::File* scriptFile = Arya::FileSystem::shared().getFile(std::string("scripts/") + filename);
    if(!scriptFile)
    {
        GAME_LOG_WARNING("Script not found: " << filename);
    }
    else
    {
        GAME_LOG_DEBUG("Executing script: " << filename);
        int errors = luaL_dostring(luaState, scriptFile->getData());
        if(errors)
            GAME_LOG_WARNING("Script " << filename << " not executed successfully");
        else
            return 1;
    }
    return 0;
}
