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

namespace Arya
{	
    Scene::Scene()
    {
        terrain = 0;
        camera = 0;
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
        terrain = new Terrain(TextureManager::shared().getTexture("heightmap.tga"),0,0);

        if(!terrain->init()) {
            LOG_WARNING("Could not load terrain");
            return false;
        }

        if(camera) delete camera;
        camera = new Camera;

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
    }

    void Scene::render()
    {
        basicProgram->use();

        for(int i = 0; i < objects.size(); ++i)
        {
            glBindVertexArray(objects[i]->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        // terrain->render();
    }
}
