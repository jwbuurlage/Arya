#include "Arya.h"
#include "../include/GameSession.h"

GameSession::GameSession()
{

}

GameSession::~GameSession()
{

}

void GameSession::start()
{
    Map* map;
    Scene* scene = root->getScene();
    scene->setMap(map);
    
    // init factions
    
    // root start rendering scene?
}
