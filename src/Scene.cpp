#include <iostream>
#include <string.h>
#include <GL/glew.h>

#include "Primitives.h"
#include "Scene.h"
#include "Terrain.h"
#include "Camera.h"

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
        ColoredTriangle* cTri = new ColoredTriangle;
        objects.push_back(cTri);

        if( terrain ) delete terrain;
        terrain = new Terrain(0,0,0);

        if( camera ) delete camera;
        camera = new Camera;

        return true;
    }

    void Scene::cleanup()
    {
        if( camera ) delete camera;
        camera = 0;
        if( terrain ) delete terrain;
        terrain = 0;
    }

    void Scene::render()
    {
        for(int i = 0; i < objects.size(); ++i)
        {
            glBindVertexArray(objects[i]->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        terrain->render();
    }
}
