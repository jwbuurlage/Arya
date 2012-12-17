#include "../include/ServerClient.h"
#include "../include/Units.h"

void ServerClient::createFaction()
{
    faction = new Faction;
    faction->setId(clientId);
    faction->setColor(clientId%5);
}

UnitInfo unitinfo;

void ServerClient::createStartUnits()
{
    if(!faction) return;

    UnitInfo* info = &unitinfo;
    info->radius = 5.0f;
    info->maxHealth = 100.0f;
    info->speed = 30.0f;
    info->yawSpeed = 720.0f;
    info->damage = 20.0f;
    info->attackSpeed = 1.0f;

    for(int i = 0; i < 30; ++ i) 
    {
        Unit* unit = new Unit(info);
        unit->setPosition(vec3(20 * (i / 10), 10, -50 + 20 * (i % 10)));
        faction->addUnit(unit);
    }
    return;
}
