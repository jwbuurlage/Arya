#include "../include/UnitTypes.h"
#include "../include/common/GameLogger.h"
#include <map>

using std::map;

map<int,UnitInfo*> unitInfoList;

UnitInfo* getUnitInfo(int type)
{
    map<int,UnitInfo*>::iterator iter = unitInfoList.find(type);
    if(iter == unitInfoList.end())
        return 0;
    else
        return iter->second;
}

UnitInfo* getUnitInfo(const string& name)
{
    map<int,UnitInfo*>::iterator iter;
    for(iter = unitInfoList.begin(); iter != unitInfoList.end(); ++iter)
        if(iter->second->displayname == name || iter->second->modelname == name)
            return iter->second;
    return 0;
}

void registerNewUnitInfo(UnitInfo* info)
{
    unitInfoList[info->typeId] = info;
}

UnitInfo::UnitInfo(int type) : typeId(type)
{
    registerNewUnitInfo(this);
    //default values
    animationIdle = "stand";
    animationMove = "run";
    animationAttack = "attack";
    animationAttackOutOfRange = "crouch_walk";
    animationDie = "death_fallback";
}

UnitInfo::~UnitInfo()
{
    //TODO: remove from unitInfoList
}

