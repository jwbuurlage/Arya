#include "../include/Scripting.h"
#include "../include/UnitTypes.h"
#include "../include/Units.h"
#include "../include/common/GameLogger.h"
#include <luabind/luabind.hpp>
#include <lua.hpp>

//TODO: move Scripting::execute to another cpp file
//so that we dont need to inclue Arya.h here
#include "../include/Files.h"

class LuaUnitType : public UnitInfo
{
    public:
        //registers itself as a unit type
        LuaUnitType(int typeId) : UnitInfo(typeId) {};
        ~LuaUnitType() {};

        //These are called by the game when the event occurs
        void onDeath(Unit* unit);
        void onSpawn(Unit* unit);
        void onDamage(Unit* victim, Unit* attacker, float damage);

        //These are called by the scripts to set handlers
        void setOnDeath(const luabind::object& obj);
        void setOnSpawn(const luabind::object& obj);
        void setOnDamage(const luabind::object& obj);

    private:
        luabind::object objOnDeath;
        luabind::object objOnSpawn;
        luabind::object objOnDamage;
};

void LuaUnitType::onDeath(Unit* unit)
{
    if(objOnDeath) try{ luabind::call_function<void>(objOnDeath, unit); }catch(luabind::error& e){ GAME_LOG_ERROR("Script error: " << e.what()); }
}
void LuaUnitType::onSpawn(Unit* unit)
{
    if(objOnSpawn) try{ luabind::call_function<void>(objOnSpawn, unit); }catch(luabind::error& e){ GAME_LOG_ERROR("Script error: " << e.what()); }
}
void LuaUnitType::onDamage(Unit* victim, Unit* attacker, float damage)
{
    if(objOnDamage) try{ luabind::call_function<void>(objOnDamage, victim, attacker, damage); }catch(luabind::error& e){ GAME_LOG_ERROR("Script error: " << e.what()); }
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
    luaL_openlibs(luaState);

    luabind::open(luaState);

    luabind::object pak = luabind::globals(luaState)["package"];
    if(pak)
    {
        luabind::object pathobj = pak["path"];
        if(pathobj)
        {
            std::string curpath = luabind::object_cast<std::string>(pak["path"]);
            curpath.append(";");
            curpath.append(Arya::FileSystem::shared().getApplicationPath());
            curpath.append("scripts/?");
            curpath.append(";");
            curpath.append(Arya::FileSystem::shared().getApplicationPath());
            curpath.append("scripts/?.lua");
            pak["path"] = curpath;
        }
        else
            GAME_LOG_WARNING("Lua object package.path not found");
    }
    else
        GAME_LOG_WARNING("No global called 'package' was found");

    //For properties: when no set function is given it is readonly
    luabind::module(luaState)[
        luabind::def("print", &luaPrint),
        luabind::class_<Unit>("Unit") //scripts may not create these, so no constructor
            .property("id", &Unit::getId)
            .property("type", &Unit::getType)
            .property("health", &Unit::getHealth),
        luabind::class_<UnitInfo>("UnitInfoBase"), //should not be used in scripts directly
        luabind::class_<LuaUnitType, UnitInfo>("UnitType")
            .def(luabind::constructor<int>())
            .def("setOnDeath", &LuaUnitType::setOnDeath)
            .def("setOnSpawn", &LuaUnitType::setOnSpawn)
            .def("setOnDamage", &LuaUnitType::setOnDamage)
            .def_readwrite("displayname", &LuaUnitType::displayname)
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
        int err = luaL_loadbuffer(luaState, scriptFile->getData(), scriptFile->getSize(), filename);
        if(err == 0) err = lua_pcall(luaState, 0, LUA_MULTRET, 0);
        if(err == 0) return 1;
        GAME_LOG_WARNING("Script " << filename << " error: " << lua_tostring(luaState, -1));
        lua_pop(luaState, 1); //pop the error message from the lua stack
    }
    return 0;
}
