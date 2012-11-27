#include <iostream>
#include <string.h>
#include <GL/glew.h>

#include "Primitives.h"
#include "Scene.h"

using std::string;
using std::cerr;
using std::endl;

namespace Arya
{	
    Scene::Scene()
    {
        init();
    }

    bool Scene::init()
    {
        ColoredTriangle* cTri = new ColoredTriangle;
        objects.push_back(cTri);
        return true;
    }

    void Scene::render()
    {
        for(int i = 0; i < objects.size(); ++i)
        {
            glBindVertexArray(objects[i]->getVAO());
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
    }
}
