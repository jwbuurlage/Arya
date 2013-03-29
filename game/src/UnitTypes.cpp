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

//Default unit types for when the scripts wont load

//           id  name      radius  atRad  viewRad   speed   yawSpeed health   damage  attackSpeed   moveWhileAttacking   selectionSound   attackSound
UnitInfo ogre(0, "ogros",   10.0f,  5.0f,  50.0f,    30.0f,  720.0f,  100.0f,  20.0f,  1.0f,         true,                "yes.wav",       "yes.wav");
UnitInfo hep (1, "hep",     15.0f,  50.0f, 80.0f,    50.0f,  360.0f,  60.0f,   10.0f,  0.7f,         false,               "yes.wav",       "yes.wav");
UnitInfo imp (2, "imp",     30.0f,  15.0f, 10.0f,    10.0f,  180.0f,  300.0f,   50.0f,  0.1f,        false,               "chewbacca.wav", "noMercy.wav");


