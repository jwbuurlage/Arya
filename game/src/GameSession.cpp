#include "Arya.h"
#include "../include/GameSession.h"
#include "../include/GameSessionInput.h"
#include "../include/Faction.h"

GameSession::GameSession()
{
        input = 0;
}

GameSession::~GameSession()
{
    if(input) {
        Root::shared().removeInputListener(input);
        Root::shared().removeFrameListener(input);
        delete input;
    }

    Root::shared().removeScene();

    LOG_INFO("Ended session");
}

bool GameSession::init()
{
    input = new GameSessionInput(this);
    input->init();

    Root::shared().addInputListener(input);
    Root::shared().addFrameListener(input); 

    // init factions
    localFaction = new Faction;
    factions.push_back(localFaction);

    Scene* scene = Root::shared().makeDefaultScene();
    if(!scene)
        return false;
    Object* obj;

    obj = scene->createObject();
    obj->setModel(ModelManager::shared().getModel("triangle"));
    obj->setPosition(vec3(0, 0, 0));

    localFaction->addUnit(obj);

    obj = scene->createObject();
    obj->setModel(ModelManager::shared().getModel("quad"));
    obj->setPosition(vec3(0, 0, 5));

    localFaction->addUnit(obj);

    // init map
    vector<Texture*> tileSet;
    tileSet.push_back(TextureManager::shared().getTexture("grass.tga"));
    tileSet.push_back(TextureManager::shared().getTexture("rock.tga"));
    tileSet.push_back(TextureManager::shared().getTexture("dirt.tga"));
    tileSet.push_back(TextureManager::shared().getTexture("snow.tga"));
    if(!scene->setMap("heightmap.raw", tileSet, TextureManager::shared().getTexture("splatmap.tga")))
        return false;

    return true;
}
