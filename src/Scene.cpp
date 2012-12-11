#include <iostream>
#include <string.h>
#include <GL/glew.h>

#include "common/Listeners.h"
#include "common/Logger.h"
#include "Objects.h"
#include "Models.h"
#include "Scene.h"
#include "Fonts.h"
#include "Map.h"
#include "Terrain.h"
#include "Textures.h"
#include "Camera.h"
#include "Shaders.h"

using std::string;
using std::cerr;
using std::endl;
using std::vector;

namespace Arya
{	
    Scene::Scene()
    {
        initialized = false;
        currentMap = 0; 
        camera = 0;
        basicProgram = 0;
        init();
    }

    Scene::~Scene()
    {
        cleanup();
    }

    Object* Scene::createObject()
    {
        Object* obj = new Object;
        objects.push_back(obj);
        return obj;
    }

    bool Scene::init()
    {
        if(!initShaders()) return false;

        LOG_INFO("Loading scene");

        if(!camera)
        {
            camera = new Camera;
            camera->setProjectionMatrix(45.0f, 1.6f, 0.1f, 2000.0f);
        }

        initialized = true;

        return true;
    }

    bool Scene::initShaders()
    {
        Shader* basicVertex = new Shader(VERTEX);
        if(!(basicVertex->addSourceFile("../shaders/basic.vert"))) return false;
        if(!(basicVertex->compile())) return false;

        Shader* basicFragment = new Shader(FRAGMENT);
        if(!(basicFragment->addSourceFile("../shaders/basic.frag"))) return false;
        if(!(basicFragment->compile())) return false;

        basicProgram = new ShaderProgram("basic");
        basicProgram->attach(basicVertex);
        basicProgram->attach(basicFragment);
        if(!(basicProgram->link())) return false;

        return true;
    }

    void Scene::setMap(const char* hm, vector<Texture*> ts, Texture* sm)
    {
        if(currentMap)
            delete currentMap;
        currentMap = new Map(hm, ts, sm);
    }

    void Scene::cleanup()
    {
        if(camera) delete camera;
        camera = 0;

        if(currentMap) delete currentMap;
        currentMap = 0;

        if(basicProgram) delete basicProgram;
        basicProgram = 0;

        for(int i = 0; i < objects.size(); ++i)
            delete objects[i];
        objects.clear();

        initialized = false;
    }

    void Scene::onFrame(float elapsedTime)
    {
        camera->update(elapsedTime);
        currentMap->update(elapsedTime, this);
    }

    void Scene::render()
    {
        basicProgram->use();

        mat4 vpMatrix;
        camera->updateViewProjectionMatrix(&vpMatrix);
        basicProgram->setUniformMatrix4fv("vpMatrix", vpMatrix);
        basicProgram->setUniform1i("texture1",0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, FontManager::shared().getFont("courier.ttf")->textureHandle);

        for(int i = 0; i < objects.size(); ++i)
        {
            if( objects[i]->model == 0 ) continue;
            basicProgram->setUniformMatrix4fv("mMatrix", objects[i]->getMoveMatrix());
            Model* model = objects[i]->model;
            for(int j = 0; j < model->getMeshes().size(); ++j)
            {
                Mesh* mesh = model->getMeshes()[j];
                glBindVertexArray(mesh->vaoHandle);
                glDrawArrays(mesh->primitiveType, 0, mesh->vertexCount);
            }
        }

        currentMap->render(camera);
    }
}
