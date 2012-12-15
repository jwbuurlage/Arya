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
#include "Interface.h"

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
            camera->setProjectionMatrix(45.0f, Root::shared().getAspectRatio(), 0.1f, 2000.0f);
        }

        initialized = true;

        return true;
    }

    bool Scene::initShaders()
    {
        Shader* basicVertex = new Shader(VERTEX);
        if(!(basicVertex->addSourceFile("../shaders/staticmodel.vert"))) return false;
        if(!(basicVertex->compile())) return false;

        Shader* basicFragment = new Shader(FRAGMENT);
        if(!(basicFragment->addSourceFile("../shaders/staticmodel.frag"))) return false;
        if(!(basicFragment->compile())) return false;

        basicProgram = new ShaderProgram("basic");
        basicProgram->attach(basicVertex);
        basicProgram->attach(basicFragment);
        if(!(basicProgram->link())) return false;

        return true;
    }

    bool Scene::setMap(const char* hm, vector<Texture*> ts, Texture* sm)
    {
        if(currentMap)
            delete currentMap;
        currentMap = new Map();
        if(!currentMap->init(hm, ts, sm)) return false;
        return true;
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
        //We might want to let the Game loop on all
        //the objects and animate them. This way the game
        //has more control: it could slow animations down for example
        for(int i = 0; i < objects.size(); ++i)
        {
            objects[i]->updateAnimation(elapsedTime);
        }
    }

    void Scene::render()
    {
        basicProgram->use();

        basicProgram->setUniformMatrix4fv("vpMatrix", camera->getVPMatrix());

        for(int i = 0; i < objects.size(); ++i)
        {
            if( objects[i]->model == 0 ) continue;
            basicProgram->setUniform3fv("tintColor", objects[i]->getTintColor());

            basicProgram->setUniformMatrix4fv("mMatrix", objects[i]->getMoveMatrix());

            int frame = 0;
            float interpolation = 0.0f;
            AnimationState* animState = objects[i]->getAnimationState(); //can be zero
            if(animState)
            {
                frame = animState->getCurFrame();
                interpolation = animState->getInterpolation();
                basicProgram->setUniform1f("interpolation", interpolation);
            }

            Model* model = objects[i]->model;
            for(int j = 0; j < model->getMeshes().size(); ++j)
            {
                Mesh* mesh = model->getMeshes()[j];
                if(mesh->frameCount > 0)
                {
                    Material* mat = model->getMaterials()[mesh->materialIndex];
                    basicProgram->setUniform1i("tex", 0);
                    glActiveTexture(GL_TEXTURE0);
                    if(mat) glBindTexture(GL_TEXTURE_2D, mat->handle);

                    glBindVertexArray(mesh->vaoHandles[frame]);
                    glDrawArrays(mesh->primitiveType, 0, mesh->vertexCount);
                }
            }
        }

        currentMap->render(camera);
    }
}
