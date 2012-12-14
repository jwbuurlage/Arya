#include "Arya.h"
#include "../include/GameSession.h"
#include "../include/GameSessionInput.h"
#include "../include/Faction.h"
#include "../include/Units.h"

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

    Root::shared().removeFrameListener(this);

    Root::shared().removeScene();

    LOG_INFO("Ended session");
}

bool GameSession::init()
{
    input = new GameSessionInput(this);
    input->init();

    Root::shared().addInputListener(input);
    Root::shared().addFrameListener(input);
    Root::shared().addFrameListener(this);

    // init factions
    localFaction = new Faction;
    factions.push_back(localFaction);

    Scene* scene = Root::shared().makeDefaultScene();
    if(!scene)
        return false;
    Object* obj;

    // init map
    vector<Texture*> tileSet;
    tileSet.push_back(TextureManager::shared().getTexture("grass.tga"));
    tileSet.push_back(TextureManager::shared().getTexture("rock.tga"));
    tileSet.push_back(TextureManager::shared().getTexture("dirt.tga"));
    tileSet.push_back(TextureManager::shared().getTexture("snow.tga"));
    if(!scene->setMap("heightmap.raw", tileSet, TextureManager::shared().getTexture("splatmap.tga")))
        return false;

    // TODO: This is a memleak, but we will load info in from a file somewhere
    // and this will fix this issue
    UnitInfo* info = new UnitInfo;
    info->radius = 2.0f;

    Unit* unit = new Unit(info);

    for(int i = 0; i < 10; ++ i) 
    {
        float heightModel = Root::shared().getScene()->getMap()->getTerrain()->heightAtGroundPosition(0.0, -50.0+20.0*i);
        obj = scene->createObject();
        obj->setModel(ModelManager::shared().getModel("ogros.aryamodel"));
        obj->setPosition(vec3(0, 22.0 + heightModel, -50 + 30 * i));

        unit->setObject(obj);
        localFaction->addUnit(unit);
    }

    delete unit;

    return true;
}

void GameSession::onRender()
{
    static GLfloat vertices[] = {
        0.0, 1.0,
        0.0, 0.0,
        1.0, 1.0,
        1.0, 0.0
    };

    for(int i = 0; i < localFaction->getUnits().size(); ++i)
    {
        // render circle
    }
}
