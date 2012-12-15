#include "Arya.h"
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
    {
        delete factions[i];
    }
    factions.clear();

    Root::shared().removeFrameListener(this);

    Root::shared().removeScene();

    LOG_INFO("Ended session");
}

bool GameSession::init()
{
    input = new GameSessionInput(this);
    input->init();

    if(!initShaders()) return false;
    if(!initVertices()) return false;

    Root::shared().addInputListener(input);
    Root::shared().addFrameListener(input);
    Root::shared().addFrameListener(this);

    // init factions
    localFaction = new Faction;
    factions.push_back(localFaction);

    Scene* scene = Root::shared().makeDefaultScene();
    if(!scene)
        return false;

    Camera* cam = scene->getCamera();

    cam->setPosition(vec3(0.0f, 150.0f, 0.0f));
    cam->setCameraAngle(0.0f, -60.0f);
    cam->camZoomSpeed = 80.0f;

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

    for(int i = 0; i < 100; ++ i) 
    {
        Unit* unit = new Unit(info);
        float heightModel = Root::shared().getScene()->getMap()->getTerrain()->heightAtGroundPosition(20.0 * i / 10, -50.0+20.0*(i % 10));
        obj = scene->createObject();
        obj->setModel(ModelManager::shared().getModel("ogros.aryamodel"));
        obj->setPosition(vec3(20 * i / 10, heightModel, -50 + 20 * (i % 10)));
        obj->setAnimation("run");

        unit->setObject(obj);
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
        for(int j = 0; j < factions[i]->getUnits().size(); ++j)
            factions[i]->getUnits()[j]->update(elapsedTime);
}

void GameSession::onRender()
{
    glDisable(GL_CULL_FACE);
    glEnable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);

    decalProgram->use();
    glBindVertexArray(decalVao);

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

    for(int i = 0; i < localFaction->getUnits().size(); ++i)
    {
        if(!localFaction->getUnits()[i]->isSelected()) continue;
        vec2 groundPos = vec2(localFaction->getUnits()[i]->getObject()->getPosition().x,
                localFaction->getUnits()[i]->getObject()->getPosition().z);
        decalProgram->setUniform2fv("groundPosition", groundPos);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    glDisable(GL_BLEND);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_CULL_FACE);
}
