#include "Arya.h"
#include "../include/Game.h"
#include "../include/GameSession.h"
#include "../include/GameSessionInput.h"
#include "../include/Faction.h"
#include "../include/Units.h"

GameSession::GameSession()
{
    input = 0;
    localFaction = 0;

    decalVao = 0;
    decalProgram = 0;
    selectionDecalHandle = 0;
}

GameSession::~GameSession()
{
    if(decalProgram)
        delete decalProgram;

    // TODO: delete vertex buffers

    if(input) {
        Root::shared().removeInputListener(input);
        Root::shared().removeFrameListener(input);
        delete input;
    }

    for(int i = 0; i < factions.size(); ++i)
        delete factions[i];
    factions.clear();

    Root::shared().removeFrameListener(this);

    Root::shared().removeScene();

    LOG_INFO("Ended session");
}

bool GameSession::init()
{
    Game::shared().getEventManager()->addEventHandler(EVENT_GAME_FULLSTATE, this);

    input = new GameSessionInput(this);
    input->init();

    if(!initShaders()) return false;
    if(!initVertices()) return false;

    Root::shared().addInputListener(input);
    Root::shared().addFrameListener(input);
    Root::shared().addFrameListener(this);

    // init factions
    localFaction = new Faction;
    localFaction->setColor(0);
    factions.push_back(localFaction);

    Faction* otherFaction = new Faction;
    otherFaction->setColor(1);
    factions.push_back(otherFaction);

    Scene* scene = Root::shared().makeDefaultScene();
    if(!scene)
        return false;

    Camera* cam = scene->getCamera();

    cam->setPosition(vec3(0.0f, 150.0f, 0.0f));
    cam->setCameraAngle(0.0f, -60.0f);
    cam->setZoom(300.0f);

    Object* obj;

    // init map
    vector<Arya::Material*> tileSet;
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("grass.tga"));
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("rock.tga"));
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("snow.tga"));
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("dirt.tga"));
    if(!scene->setMap("heightmap.raw", "watermap.raw", tileSet, Arya::TextureManager::shared().getTexture("clouds.jpg"), Arya::TextureManager::shared().getTexture("splatmap.tga")))
        return false;

    // TODO: This is a memleak, but we will load info in from a file somewhere
    // and this will fix this issue
    UnitInfo* info = new UnitInfo;
    info->radius = 5.0f;
    info->attackRadius = 5.0f;
    info->maxHealth = 100.0f;
    info->speed = 30.0f;
    info->yawSpeed = 720.0f;
    info->damage = 20.0f;
    info->attackSpeed = 1.0f;

    for(int i = 0; i < 30; ++ i) 
    {
        Unit* unit = new Unit(info);
        float heightModel = Root::shared().getScene()->getMap()->getTerrain()->heightAtGroundPosition(20.0 * (i / 10), -50.0+20.0*(i % 10));
        obj = scene->createObject();
        obj->setModel(ModelManager::shared().getModel("ogros.aryamodel"));
        obj->setAnimation("stand");
        unit->setObject(obj);
        unit->setPosition(vec3(20 * (i / 10), heightModel, -50 + 20 * (i % 10)));

        localFaction->addUnit(unit);
    }

    for(int i = 0; i < 30; ++ i) 
    {
        Unit* unit = new Unit(info);
        float heightModel = Root::shared().getScene()->getMap()->getTerrain()->heightAtGroundPosition(-100.0 + 20.0 * (i / 10), -100.0+20.0*(i % 10));
        obj = scene->createObject();
        obj->setModel(ModelManager::shared().getModel("ogros.aryamodel"));
        obj->setAnimation("stand");
        unit->setObject(obj);
        unit->setPosition(vec3(-100.0 + 20 * (i / 10), heightModel, -100.0 + 20 * (i % 10)));

        otherFaction->addUnit(unit);
    }

    info = new UnitInfo;
    info->radius = 5.0f;
    info->attackRadius = 50.0f;
    info->maxHealth = 60.0f;
    info->speed = 30.0f;
    info->yawSpeed = 720.0f;
    info->damage = 30.0f;
    info->attackSpeed = 1.0f;

    for(int i = 0; i < 10; ++ i) 
    {
        Unit* unit = new Unit(info);
        float heightModel = Root::shared().getScene()->getMap()->getTerrain()->heightAtGroundPosition(-200.0 + 20.0 * (i / 10), -50.0+20.0*(i % 10));
        obj = scene->createObject();
        obj->setModel(ModelManager::shared().getModel("hep.aryamodel"));
        obj->setAnimation("stand");
        unit->setObject(obj);
        unit->setPosition(vec3(-200.0 + 20 * (i / 10), heightModel, -50 + 20 * (i % 10)));

        localFaction->addUnit(unit);
    }

    selectionDecalHandle = 0;
    Texture* selectionTex = TextureManager::shared().getTexture("selection.png");
    if(selectionTex) selectionDecalHandle = selectionTex->handle;

    return true;
}

bool GameSession::initShaders()
{
    Shader* decalVertex = new Shader(Arya::VERTEX);
    if(!(decalVertex->addSourceFile("../shaders/terraindecal.vert"))) return false;
    if(!(decalVertex->compile())) return false;

    Shader* decalFragment = new Shader(Arya::FRAGMENT);
    if(!(decalFragment->addSourceFile("../shaders/terraindecal.frag"))) return false;
    if(!(decalFragment->compile())) return false;

    decalProgram = new ShaderProgram("terraindecal");
    decalProgram->attach(decalVertex);
    decalProgram->attach(decalFragment);
    if(!(decalProgram->link())) return false;

    return true;
}

bool GameSession::initVertices()
{
    GLfloat vertices[] = {
        0.0, 0.0,
        1.0, 0.0,
        0.0, 1.0,
        1.0, 1.0
    };

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW); 

    glGenVertexArrays(1, &decalVao);
    glBindVertexArray(decalVao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, false, 0, (void*)0);

    glBindVertexArray(0);

    return true;
}

void GameSession::onFrame(float elapsedTime)
{
    // update units
    for(int i = 0; i < factions.size(); ++i)
    {
        for(list<Unit*>::iterator it = factions[i]->getUnits().begin();
                it != factions[i]->getUnits().end(); )
        {
            if((*it)->obsolete() && (*it)->readyToDelete()) {
                delete *it;
                it = factions[i]->getUnits().erase(it);
            }
            else {
                mat4 vpMatrix = Root::shared().getScene()->getCamera()->getVPMatrix();

                vec4 onScreen((*it)->getObject()->getPosition(), 1.0);
                onScreen = vpMatrix * onScreen;
                onScreen.x /= onScreen.w;
                onScreen.y /= onScreen.w;
                (*it)->setScreenPosition(vec2(onScreen.x, onScreen.y));

                (*it)->update(elapsedTime);
                ++it;
            }
        }
    }
}

void GameSession::onRender()
{
    glDisable(GL_CULL_FACE);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);

    decalProgram->use();
    glBindVertexArray(decalVao);

    decalProgram->setUniform1f("yOffset", 0.5);
    decalProgram->setUniformMatrix4fv("vpMatrix", Root::shared().getScene()->getCamera()->getVPMatrix());
    decalProgram->setUniformMatrix4fv("scaleMatrix", Root::shared().getScene()->getMap()->getTerrain()->getScaleMatrix());

    // heightmap
    decalProgram->setUniform1i("heightMap", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Root::shared().getScene()->getMap()->getTerrain()->getHeightMapHandle());

    // selection
    decalProgram->setUniform1i("selectionTexture", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, selectionDecalHandle);

    decalProgram->setUniform3fv("uColor", localFaction->getColor());

    for(list<Unit*>::iterator it = localFaction->getUnits().begin();
            it != localFaction->getUnits().end(); ++it)
    {
        if(!((*it)->isSelected()))
            continue;

        vec2 groundPos = vec2((*it)->getObject()->getPosition().x,
                (*it)->getObject()->getPosition().z);
        decalProgram->setUniform2fv("groundPosition", groundPos);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_CULL_FACE);
}

void GameSession::handleEvent(Packet& packet)
{
    int id = packet.getId();
    switch(id) 
    {
        case EVENT_GAME_FULLSTATE:
            int playerCount;
            packet >> playerCount;

            LOG_INFO("Game has " << playerCount << " player(s)");

            for(int i = 0; i < playerCount; ++i)
            {
                int clientId;
                packet >> clientId;

                //faction deserialize

                int unitCount;
                packet >> unitCount;
                for(int i = 0; i < unitCount; ++i)
                {
                    //create unit
                    //deserialize
                }
            }
            break;

        default:
            LOG_INFO("GameSession: unknown event received! (" << id << ")");
            break;
    }

}
