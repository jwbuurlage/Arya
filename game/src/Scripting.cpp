#include "../include/Scripting.h"
#include "../include/Game.h"
#include "../include/GameSession.h"
#include "../include/UnitTypes.h"
#include "../include/Units.h"
#include "../include/Faction.h"
#include "../include/MapInfo.h"
#include "../include/ServerGameSession.h"
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
    Scripting* scripting = session->getScripting();
    if(scripting)
        customData = new LuaScriptData(luabind::newtable(scripting->getState()));
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
				2048.0f,
				2048.0f,
				"Borderlands",
				"borderlands_heightmap.raw",
				1025,
				"borderlands_splatmap.tga",
				"grass.tga,snow.tga,rock.tga,dirt.tga");

//TODO: Make this thread-local if server becomes multi-threaded
ServerGameSession* callbackSession = 0;

class LuaMapInfo : public MapInfo
{
    public:
        LuaMapInfo(int id) : MapInfo(id) {}
        ~LuaMapInfo(){}

        void onLoad(ServerGameSession* serversession)
        {
            if(objOnLoad && luabind::type(objOnLoad) == LUA_TFUNCTION)
            {
                ServerGameSession* oldsession = callbackSession;
                callbackSession = serversession;
                try{ luabind::call_function<void>(objOnLoad); }
                catch(luabind::error& e){ GAME_LOG_ERROR("Script error: " << e.what()); }
                callbackSession = oldsession;
            }
        }

        void onLoadFaction(ServerGameSession* serversession, int factionId, int factionSpawnPos)
        {
            if(objOnLoadFaction && luabind::type(objOnLoadFaction) == LUA_TFUNCTION)
            {
                ServerGameSession* oldsession = callbackSession;
                callbackSession = serversession;
                try{ luabind::call_function<void>(objOnLoadFaction, factionId, factionSpawnPos); }
                catch(luabind::error& e){ GAME_LOG_ERROR("Script error: " << e.what()); }
                callbackSession = oldsession;
            }
        }

        luabind::object objOnLoad;
        luabind::object objOnLoadFaction;
};

LuaMapInfo* createMap(int typeId)
{
    return new LuaMapInfo(typeId);
}

class LuaVec2
{
    public:
        LuaVec2(float _x, float _y) : x(_x), y(_y) {}
        ~LuaVec2() {}
        float x, y;
};
class LuaVec3
{
    public:
        LuaVec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
        ~LuaVec3() {}
        float x, y, z;
};

int createFaction()
{
    if(callbackSession == 0)
    {
        GAME_LOG_WARNING("Script called createFaction but no game session was set");
        return 0;
    }
    else
    {
		Faction* faction = callbackSession->createFaction();
        return faction->getId();
    }
}

void spawnUnit(int factionId, const std::string& unitType, const LuaVec2& pos)
{
    if(callbackSession == 0)
    {
        GAME_LOG_WARNING("Script called spawnUnit but no game session was set");
    }
    else
    {
        Faction* faction = callbackSession->getFactionById(factionId);
        if(!faction)
        {
            GAME_LOG_WARNING("Script called spawnUnit with invalid faction id");
        }
        else
        {
            UnitInfo* info = getUnitInfo(unitType);
            if(!info)
            {
                GAME_LOG_WARNING("Script called spawnUnit with invalid unit type");
            }
            else
            {
                Unit* unit = callbackSession->createUnit(info->typeId);
                unit->setPosition(vec3(pos.x, 0, pos.y));
                faction->addUnit(unit);
                //Call the unit script callback
                unit->getInfo()->onSpawn(unit);
            }
        }
    }
}

void getUnitsNearLocation(vec2 location)
{

}

//
// ----------------------------------
//

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

    //Set the script search path for the require function
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

        luabind::class_<LuaVec2>("vec2")
            .def(luabind::constructor<float,float>())
            .def_readwrite("x", &LuaVec2::x)
            .def_readwrite("y", &LuaVec2::y),
        luabind::class_<LuaVec3>("vec3")
            .def(luabind::constructor<float,float, float>())
            .def_readwrite("x", &LuaVec3::x)
            .def_readwrite("y", &LuaVec3::y)
            .def_readwrite("z", &LuaVec3::z),

        luabind::def("createFaction", &createFaction),
        luabind::def("spawnUnit", &spawnUnit),
        luabind::def("getUnitsNearLocation", &getUnitsNearLocation),

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
            .def_readwrite("onLoadFaction", &LuaMapInfo::objOnLoadFaction)
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
