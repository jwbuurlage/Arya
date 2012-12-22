#include "../include/ServerClient.h"
#include "../include/ServerGameSession.h"
#include "../include/Units.h"

ServerClient::~ServerClient()
{
    if(gameSession) gameSession->removeClient(this);
    if(faction) delete faction;
}

void ServerClient::createFaction()
{
    if(!faction) faction = new Faction;
    faction->setId(clientId);
    faction->setColor(clientId%5);
}

void ServerClient::createStartUnits()
{
    if(!faction) return;

    for(int i = 0; i < 20; ++ i) 
    {
        Unit* unit = gameSession->createUnit(0);
        unit->setPosition(vec3(-(faction->getId()%5) * 100.0f + 20.0f * (i / 10), 0.0f, -50.0f + 20.0f * (i % 10)));
        faction->addUnit(unit);

        LOG_INFO("TEST UNIT ID " << unit->getId());

        unit = gameSession->createUnit(1);
        unit->setPosition(vec3(-(faction->getId()%5) * 100.0f - 50.0f + 20.0f * (i / 10), 0.0f, -50.0f + 20.0f * (i % 10)));
        faction->addUnit(unit);
    }
    return;
}
