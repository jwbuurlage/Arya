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

    for(unsigned int i = 0; i < factions.size(); ++i)
        delete factions[i];
    factions.clear();

    Root::shared().removeFrameListener(this);

    Root::shared().removeScene();

    LOG_INFO("Ended session");
}

bool GameSession::init()
{
    Game::shared().getEventManager()->addEventHandler(EVENT_CLIENT_CONNECTED, this);
    Game::shared().getEventManager()->addEventHandler(EVENT_CLIENT_DISCONNECTED, this);
    //Game::shared().getEventManager()->addEventHandler(EVENT_GAME_FULLSTATE, this);
    Game::shared().getEventManager()->addEventHandler(EVENT_MOVE_UNIT, this);
    Game::shared().getEventManager()->addEventHandler(EVENT_ATTACK_MOVE_UNIT, this);

    input = new GameSessionInput(this);
    input->init();

    if(!initShaders()) return false;
    if(!initVertices()) return false;

    Root::shared().addInputListener(input);
    Root::shared().addFrameListener(input);
    Root::shared().addFrameListener(this);

    Scene* scene = Root::shared().makeDefaultScene();
    if(!scene)
        return false;

    Camera* cam = scene->getCamera();

    cam->setPosition(vec3(0.0f, 150.0f, 0.0f));
    cam->setCameraAngle(0.0f, -60.0f);
    cam->setZoom(300.0f);

    // init map
    vector<Arya::Material*> tileSet;
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("grass.tga"));
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("rock.tga"));
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("snow.tga"));
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("dirt.tga"));
    if(!scene->setMap("heightmap.raw", "watermap.raw", tileSet, Arya::TextureManager::shared().getTexture("clouds.jpg"), Arya::TextureManager::shared().getTexture("splatmap.tga")))
        return false;

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
    mat4 vpMatrix = Root::shared().getScene()->getCamera()->getVPMatrix();
    for(unsigned int i = 0; i < factions.size(); ++i)
    {
        for(list<Unit*>::iterator it = factions[i]->getUnits().begin();
                it != factions[i]->getUnits().end(); )
        {
            if((*it)->obsolete() && (*it)->readyToDelete()) {
                delete *it;
                it = factions[i]->getUnits().erase(it);
            }
            else {
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
        case EVENT_CLIENT_CONNECTED:
            {
                int clientId;
                packet >> clientId;

                //faction deserialize
                Faction* faction = new Faction;
                faction->deserialize(packet);
                factions.push_back(faction);

                if(clientId == Game::shared().getClientId())
                    localFaction = faction;

                int unitCount;
                packet >> unitCount;
                for(int i = 0; i < unitCount; ++i)
                {
                    Unit* unit = new Unit(0);
                    unit->deserialize(packet);

                    Object* obj = Root::shared().getScene()->createObject();
                    string s(infoForUnitType[unit->getType()].name);
                    obj->setModel(ModelManager::shared().getModel(s + ".aryamodel"));
                    obj->setAnimation("stand");

                    unit->setObject(obj);

                    float heightModel = Root::shared().getScene()->getMap()->getTerrain()->heightAtGroundPosition(
                            unit->getPosition().x, unit->getPosition().z);

                    unit->setPosition(vec3(unit->getPosition().x,
                                heightModel,
                                unit->getPosition().z));

                    faction->addUnit(unit);
                }
            }
            break;

        case EVENT_CLIENT_DISCONNECTED:
            {
                int id;
                packet >> id;
                //TODO: look up factions with this ClientID
                //Currently this is the same as faction ID
                for(unsigned int i = 0; i < factions.size(); ++i)
                {
                    if( factions[i]->getId() == id )
                    {
                        delete factions[i];
                    }
                }
            }
            break;

        case EVENT_MOVE_UNIT: {
            int facId;
            packet >> facId;

            Faction* f;

            for(int i = 0; i < factions.size(); ++i)
                if(factions[i]->getId() == facId)
                    f = factions[i];

            int numUnits;
            packet >> numUnits;

            int unitId;
            vec2 unitTargetPosition;
            // TODO: need to optimize with map
            for(int i = 0; i < numUnits; ++i) {
                packet >> unitId;
                packet >> unitTargetPosition;
                for(list<Unit*>::iterator it = f->getUnits().begin();
                        it != f->getUnits().end(); ++it)
                    if((*it)->getId() == unitId)
                        (*it)->setTargetPosition(unitTargetPosition);
            }

            break;
        }

        case EVENT_ATTACK_MOVE_UNIT: {
            int facId;
            packet >> facId;

            Faction* f;

            for(int i = 0; i < factions.size(); ++i)
                if(factions[i]->getId() == facId)
                    f = factions[i];

            int targetId;
            packet >> targetId;
            Unit* targetUnit;

            for(int i = 0; i < factions.size(); ++i)
                for(list<Unit*>::iterator it = factions[i]->getUnits().begin();
                        it != factions[i]->getUnits().end(); ++it)
                    if((*it)->getId() == targetId)
                            targetUnit = (*it);

            int numUnits;
            packet >> numUnits;

            int unitId;
            for(int i = 0; i < numUnits; ++i) {
                packet >> unitId;
                for(list<Unit*>::iterator it = f->getUnits().begin();
                        it != f->getUnits().end(); ++it)
                    if((*it)->getId() == unitId)
                            (*it)->setTargetUnit(targetUnit);
            }

            break;
         }

        default:
            LOG_INFO("GameSession: unknown event received! (" << id << ")");
            break;
    }

}
