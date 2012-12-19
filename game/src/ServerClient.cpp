#include "../include/ServerClient.h"
#include "../include/Units.h"

void ServerClient::createFaction()
{
    faction = new Faction;
    faction->setId(clientId);
    faction->setColor(clientId%5);
}

void ServerClient::createStartUnits()
{
    if(!faction) return;

    for(int i = 0; i < 30; ++ i) 
    {
        Unit* unit = new Unit(0);
        unit->setPosition(vec3(20 * (i / 10), 10, -50 + 20 * (i % 10)));
        faction->addUnit(unit);
    }
    return;
}
