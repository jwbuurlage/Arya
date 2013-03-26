#include "../include/Scripting.h"
#include "../include/Game.h"
#include "../include/UnitTypes.h"
#include "../include/Units.h"
#include "../include/MapInfo.h"
#include "../include/common/GameLogger.h"
#include <luabind/luabind.hpp>
#include <lua.hpp>

//TODO: move Scripting::execute to another cpp file
//so that we dont need to inclue Arya.h here
#include "../include/Files.h"

//
// ------------- Unit Info -------------
//

class LuaUnitType : public UnitInfo
{
    public:
        //registers itself as a unit type
        LuaUnitType(int typeId) : UnitInfo(typeId) {};
        ~LuaUnitType() { GAME_LOG_DEBUG("LuaUnitType " << displayname << " deconstructed!"); }

        //These are called by the game when the event occurs
        void onDeath(Unit* unit);
        void onSpawn(Unit* unit);
        void onDamage(Unit* victim, Unit* attacker, float damage);

        //These are set by the script, they represent functions
        luabind::object objOnDeath;
        luabind::object objOnSpawn;
        luabind::object objOnDamage;
};

LuaUnitType* createLuaUnitType(int type)
{
    //registers itself into unittype list at constructor
    return new LuaUnitType(type);
}

void LuaUnitType::onDeath(Unit* unit)
{
    if(objOnDeath && luabind::type(objOnDeath) == LUA_TFUNCTION) try{ luabind::call_function<void>(objOnDeath, unit); }catch(luabind::error& e){ GAME_LOG_ERROR("Script error: " << e.what()); }
}
void LuaUnitType::onSpawn(Unit* unit)
{
    if(objOnSpawn && luabind::type(objOnSpawn) == LUA_TFUNCTION) try{ luabind::call_function<void>(objOnSpawn, unit); }catch(luabind::error& e){ GAME_LOG_ERROR("Script error: " << e.what()); }
}
void LuaUnitType::onDamage(Unit* victim, Unit* attacker, float damage)
{
    if(objOnDamage && luabind::type(objOnDamage) == LUA_TFUNCTION) try{ luabind::call_function<void>(objOnDamage, victim, attacker, damage); }catch(luabind::error& e){ GAME_LOG_ERROR("Script error: " << e.what()); }
}

//This is a member of the Unit class
class LuaScriptData
{
    public:
        LuaScriptData(const luabind::object& obj) : luaobject(obj) {}
        ~LuaScriptData() {}

        luabind::object luaobject;
};

luabind::object& getCustomUnitData(const Unit& unit)
{
    return unit.customData->luaobject;
}

void Unit::createScriptData()
{
    customData = 0;

    Scripting& scripting = Scripting::shared();
    if(&scripting != 0)
    {
        customData = new LuaScriptData(luabind::newtable(scripting.getState()));
    }
}

void Unit::deleteScriptData()
{
    delete customData;
    customData = 0;
}

//
// ------------- Map Info -------------
//

//This is temporary, see MapInfo.h
MapInfo* theMap = new MapInfo(0, 4,
				1024.0f,
				1024.0f,
				"Borderlands",
				"heightmap.raw",
				1025,
				"splatmap.tga",
				"grass.tga,snow.tga,rock.tga,dirt.tga");

class LuaMapInfo : public MapInfo
{
    public:
        LuaMapInfo(int id) : MapInfo(id) {}
        ~LuaMapInfo(){}

        void onLoad()
        {
            if(objOnLoad && luabind::type(objOnLoad) == LUA_TFUNCTION)
            {
                try{ luabind::call_function<void>(objOnLoad); }
                catch(luabind::error& e){ GAME_LOG_ERROR("Script error: " << e.what()); }
            }
        }

        luabind::object objOnLoad;
};

LuaMapInfo* createMap(int typeId)
{
    return new LuaMapInfo(typeId);
}

//
// ----------------------------------
//

void luaPrint(const std::string& msg)
{
    GAME_LOG_DEBUG("Script: " << msg);
}

Scripting* Scripting::singleton = 0;

Scripting::Scripting()
{
    if(singleton == 0) singleton = this;
    luaState = 0;
}

Scripting::~Scripting()
{
    cleanup();
    if(singleton == this) singleton = 0;
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
            .property("health", &Unit::getHealth)
            .property("customData", &getCustomUnitData),

        luabind::def("createUnitType", &createLuaUnitType),
        luabind::class_<UnitInfo>("UnitInfoBase"), //should not be used in scripts directly
        luabind::class_<LuaUnitType, UnitInfo>("UnitType")
            //.def(luabind::constructor<int>()) //no constructor because they should be created by createUnitType
            .def_readwrite("onSpawn", &LuaUnitType::objOnSpawn)
            .def_readwrite("onDeath", &LuaUnitType::objOnDeath)
            .def_readwrite("onDamage", &LuaUnitType::objOnDamage)
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
            .def_readwrite("attackSound", &LuaUnitType::attackSound),

        luabind::def("createMap", &createMap),
        luabind::class_<MapInfo>("MapInfoBase"), //it will segfault without this line
        luabind::class_<LuaMapInfo, MapInfo>("MapInfo")
            .def_readwrite("onLoad", &LuaMapInfo::objOnLoad)
            .def_readwrite("maxPlayers", &LuaMapInfo::maxPlayers)
            .def_readwrite("width", &LuaMapInfo::width)
            .def_readwrite("height", &LuaMapInfo::height)
            .def_readwrite("name", &LuaMapInfo::name)
            .def_readwrite("heightmap", &LuaMapInfo::heightmap)
            .def_readwrite("heightmapSize", &LuaMapInfo::heightmapSize)
            .def_readwrite("splatmap", &LuaMapInfo::splatmap)
            .def_readwrite("tileset", &LuaMapInfo::tileset)
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
