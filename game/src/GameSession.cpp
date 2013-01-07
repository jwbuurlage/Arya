#include "Arya.h"
#include "../include/common/GameLogger.h"
#include "../include/Game.h"
#include "../include/GameSession.h"
#include "../include/GameSessionInput.h"
#include "../include/Map.h"
#include "../include/Faction.h"
#include "../include/Units.h"

GameSession::GameSession()
{
    input = 0;
    map = 0;
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

    if(map) delete map;

    Root::shared().removeFrameListener(this);

    Root::shared().removeScene();

    GAME_LOG_INFO("Ended session");
}

bool GameSession::init()
{
    Game::shared().getEventManager()->addEventHandler(EVENT_CLIENT_CONNECTED, this);
    Game::shared().getEventManager()->addEventHandler(EVENT_CLIENT_DISCONNECTED, this);
    Game::shared().getEventManager()->addEventHandler(EVENT_GAME_FULLSTATE, this);
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

    if(!map) map = new Map;

    if(!map->initHeightData())
        return false;
    if(!map->initGraphics(scene))
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

                (*it)->update(elapsedTime, map);
                ++it;
            }
        }
    }
}

void GameSession::onRender()
{
    if(!localFaction) return;

    glDisable(GL_CULL_FACE);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);

    decalProgram->use();
    glBindVertexArray(decalVao);

    decalProgram->setUniform1f("yOffset", 0.5);
    decalProgram->setUniformMatrix4fv("vpMatrix", Root::shared().getScene()->getCamera()->getVPMatrix());
    decalProgram->setUniformMatrix4fv("scaleMatrix", Root::shared().getScene()->getTerrain()->getScaleMatrix());

    decalProgram->setUniform1i("selectionTexture", 1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, selectionDecalHandle);

    decalProgram->setUniform3fv("uColor", localFaction->getColor());

    for(list<Unit*>::iterator it = localFaction->getUnits().begin();
            it != localFaction->getUnits().end(); ++it)
    {
        decalProgram->setUniform1f("unitRadius", (*it)->getRadius());
        if(!((*it)->isSelected()))
            continue;

        vec3 groundPos = vec3((*it)->getObject()->getPosition().x,
                map->heightAtGroundPosition((*it)->getObject()->getPosition().x, (*it)->getObject()->getPosition().z),
                (*it)->getObject()->getPosition().z);
        decalProgram->setUniform3fv("groundPosition", groundPos);
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
            {
                int count;
                packet >> count;
                for(int i = 0; i < count; ++i)
                {
                    int clientId;
                    packet >> clientId;

                    int factionId;
                    packet >> factionId;

                    Faction* faction = getFactionById(factionId);
                    if(!faction)
                    {
                        faction = createFaction(factionId);
                        factions.push_back(faction);
                    }

                    faction->deserialize(packet);

                    if(clientId == Game::shared().getClientId())
                        localFaction = faction;

                    int unitCount;
                    packet >> unitCount;
                    for(int i = 0; i < unitCount; ++i)
                    {
                        int id;
                        packet >> id;
                        Unit* unit = createUnit(id, 0);
                        unit->deserialize(packet);

                        Object* obj = Root::shared().getScene()->createObject();
                        string s(infoForUnitType[unit->getType()].name);
                        obj->setModel(ModelManager::shared().getModel(s + ".aryamodel"));
                        obj->setAnimation("stand");

                        unit->setObject(obj);

                        float heightModel = map->heightAtGroundPosition(unit->getPosition().x, unit->getPosition().z);

                        unit->setPosition(vec3(unit->getPosition().x,
                                    heightModel,
                                    unit->getPosition().z));

                        faction->addUnit(unit);
                    }
                }
            }
            break;

        case EVENT_CLIENT_CONNECTED:
            {
                int clientId;
                packet >> clientId;

                int factionId;
                packet >> factionId;

                Faction* faction = createFaction(factionId);
                faction->deserialize(packet);
                factions.push_back(faction);

                if(clientId == Game::shared().getClientId())
                    localFaction = faction;

                int unitCount;
                packet >> unitCount;
                for(int i = 0; i < unitCount; ++i)
                {
                    int id;
                    packet >> id;
                    Unit* unit = createUnit(id, 0);
                    unit->deserialize(packet);

                    Object* obj = Root::shared().getScene()->createObject();
                    string s(infoForUnitType[unit->getType()].name);
                    obj->setModel(ModelManager::shared().getModel(s + ".aryamodel"));
                    obj->setAnimation("stand");

                    unit->setObject(obj);

                    float heightModel = map->heightAtGroundPosition(unit->getPosition().x, unit->getPosition().z);

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
                for(vector<Faction*>::iterator iter = factions.begin(); iter != factions.end(); ++iter)
                {
                    if( (*iter)->getClientId() == id )
                    {
                        delete *iter;
                        iter = factions.erase(iter);
                        break;
                    }
                }
            }
            break;

        case EVENT_MOVE_UNIT: {
            int facId;
            packet >> facId;

            Faction* faction = getFactionById(facId);

            int numUnits;
            packet >> numUnits;

            int unitId;
            vec2 unitTargetPosition;
            for(int i = 0; i < numUnits; ++i) {
                packet >> unitId;
                packet >> unitTargetPosition;
                Unit* unit = getUnitById(unitId);
                if(unit) unit->setTargetPosition(unitTargetPosition);
            }

            break;
        }

        case EVENT_ATTACK_MOVE_UNIT: {
            int facId;
            packet >> facId;

            Faction* faction = getFactionById(facId);

            int numUnits;
            packet >> numUnits;

            int unitId, targetUnitId;
            for(int i = 0; i < numUnits; ++i) {
                packet >> unitId >> targetUnitId;
                Unit* unit = getUnitById(unitId);
                Unit* targetUnit = getUnitById(targetUnitId);
                if(unit && targetUnit) unit->setTargetUnit(targetUnit);
            }

            break;
         }

        default:
            GAME_LOG_INFO("GameSession: unknown event received! (" << id << ")");
            break;
    }
}

