#include <iostream>
#include <string.h>
#include <GL/glew.h>

#include "common/Logger.h"
#include "Primitives.h"
#include "Scene.h"
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
        terrain = 0;
        camera = 0;
        basicVertex = 0;
        basicFragment = 0;
        basicProgram = 0;
        init();
    }

    Scene::~Scene()
    {
        cleanup();
    }

    bool Scene::init()
    {
        if(!initShaders()) return false;

        ColoredTriangle* cTri = new ColoredTriangle;
        objects.push_back(cTri);

        if(!terrain)
        {
            vector<Texture*> tiles;
            tiles.push_back(TextureManager::shared().getTexture("grass.tga"));
            tiles.push_back(TextureManager::shared().getTexture("rock.tga"));
            tiles.push_back(TextureManager::shared().getTexture("dirt.tga"));
            tiles.push_back(TextureManager::shared().getTexture("snow.tga"));
            terrain = new Terrain(TextureManager::shared().getTexture("heightmap.tga"),tiles,0);
            if(!terrain->init()) {
                LOG_WARNING("Could not load terrain");
                delete terrain;
                terrain = 0;
                return false;
            }
        }

        if(!camera)
        {
            camera = new Camera;
            camera->setProjectionMatrix(45.0f, 1.6f, 0.1f, 50.0f);
        }

        initialized = true;

        return true;
    }

    bool Scene::initShaders()
    {
        basicVertex = new Shader(VERTEX);
        if(!(basicVertex->addSourceFile("../shaders/basic.vert"))) return false;
        if(!(basicVertex->compile())) return false;

        basicFragment = new Shader(FRAGMENT);
        if(!(basicFragment->addSourceFile("../shaders/basic.frag"))) return false;
        if(!(basicFragment->compile())) return false;

        basicProgram = new ShaderProgram("basic");
        basicProgram->attach(basicVertex);
        basicProgram->attach(basicFragment);
        if(!(basicProgram->link())) return false;

        return true;
    }

    void Scene::cleanup()
    {
        if(camera) delete camera;
        camera = 0;
        if(terrain) delete terrain;
        terrain = 0;
        if(basicVertex) delete basicVertex;
        basicVertex = 0;
        if(basicFragment) delete basicFragment;
        basicFragment = 0;
        if(basicProgram) delete basicProgram;
        basicProgram = 0;
        initialized = false;
    }

    void Scene::render()
    {
        basicProgram->use();

        mat4 vpMatrix;
        camera->updateViewProjectionMatrix(&vpMatrix);
        basicProgram->setUniformMatrix4fv("vpMatrix", vpMatrix);

        for(int i = 0; i < objects.size(); ++i)
        {
            glBindVertexArray(objects[i]->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        terrain->render(camera);
    }
}
