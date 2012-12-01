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
        terrain = 0;
        camera = 0;
        initialized = false;
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

        if(terrain) delete terrain;
        vector<Texture*> tiles;
        tiles.push_back(TextureManager::shared().getTexture("grass.tga"));
        tiles.push_back(TextureManager::shared().getTexture("rock.tga"));
        tiles.push_back(TextureManager::shared().getTexture("dirt.tga"));
        tiles.push_back(TextureManager::shared().getTexture("snow.tga"));
        terrain = new Terrain(TextureManager::shared().getTexture("heightmap.tga"),tiles,0);

        if(!terrain->init()) {
            LOG_WARNING("Could not load terrain");
            return false;
        }

        if(camera) delete camera;
        camera = new Camera;
        camera->setProjectionMatrix(45.0f, 1.6f, 0.1f, 50.0f);

        initialized = true;

        return true;
    }

    bool Scene::initShaders()
    {
        Shader* vertex = new Shader(VERTEX);
        if(!(vertex->addSourceFile("../shaders/basic.vert"))) return false;
        if(!(vertex->compile())) return false;

        Shader* fragment = new Shader(FRAGMENT);
        if(!(fragment->addSourceFile("../shaders/basic.frag"))) return false;
        if(!(fragment->compile())) return false;

        basicProgram = new ShaderProgram("basic");
        basicProgram->attach(vertex);
        basicProgram->attach(fragment);
        if(!(basicProgram->link())) return false;

        return true;
    }

    void Scene::cleanup()
    {
        if(camera) delete camera;
        camera = 0;
        if(terrain) delete terrain;
        terrain = 0;
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
