#include "../include/GameSession.h"
#include "../include/Units.h"
#include "../include/Faction.h"
#include "../include/common/GameLogger.h"

GameSession::GameSession(Scripting* _scripting, bool _server) : scripting(_scripting), isServerSession(_server)
{
    map = 0;
}

GameSession::~GameSession()
{
    //The Faction deconstructor will unregister itself
    //so the map will become empty like this
    while(!factionMap.empty())
        delete factionMap.begin()->second;

    if(!unitMap.empty())
        GAME_LOG_ERROR("List of units is not empty at deconstruction of GameSession. Possible memory leak");
}

Unit* GameSession::createUnit(int id, int type)
{
    Unit* unit = getUnitById(id);
    if(unit)
    {
        GAME_LOG_WARNING("Trying to create unit with duplicate id (" << id << ")");
        return unit;
    }
    unit = new Unit(type, id, this);
    unitMap.insert(pair<int,Unit*>(unit->getId(),unit));
    return unit;
}

void GameSession::destroyUnit(int id)
{
    unitMapIterator iter = unitMap.find(id);
    if(iter == unitMap.end())
    {
        GAME_LOG_WARNING("Trying to destory unexisting unit id");
        return;
    }
    unitMap.erase(iter);
    return;
}

Unit* GameSession::getUnitById(int id)
{
    unitMapIterator iter = unitMap.find(id);
    if(iter == unitMap.end()) return 0;
    return iter->second;
}

Faction* GameSession::createFaction(int id)
{
    Faction* faction = getFactionById(id);
    if(faction)
    {
        GAME_LOG_WARNING("Trying to create faction with duplicate id (" << id << ")");
        return faction;
    }
    faction = new Faction(id, this);
    factionMap.insert(pair<int,Faction*>(faction->getId(),faction));
    return faction;
}

void GameSession::destroyFaction(int id)
{
    factionMapIterator iter = factionMap.find(id);
    if(iter == factionMap.end())
    {
        GAME_LOG_WARNING("Trying to destory unexisting faction id");
        return;
    }
    factionMap.erase(iter);
    return;
}

Faction* GameSession::getFactionById(int id)
{
    factionMapIterator iter = factionMap.find(id);
    if(iter == factionMap.end()) return 0;
    return iter->second;
}


