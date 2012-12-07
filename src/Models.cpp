#include "Models.h"
#include "Primitives.h"

namespace Arya
{
    Model::Model()
    {

    }

    Model::~Model()
    {
        for(int i = 0; i < meshes.size(); ++i)
        {
            meshes[i]->release();
            if( meshes[i]->getRefCount() <= 0 )
            {
                delete meshes[i];
            }
        }
    }

    void Model::addMesh(Mesh* mesh)
    {
        meshes.push_back(mesh);
        mesh->addRef();
    }

    Mesh* Model::createAndAddMesh()
    {
        Mesh* newMesh = new Mesh;
        addMesh(newMesh);
        return newMesh;
    }

    template<> ModelManager* Singleton<ModelManager>::singleton = 0;

    ModelManager::ModelManager()
    {
    }

    ModelManager::~ModelManager()
    {
        cleanup();
    }

    int ModelManager::initialize()
    {
        //Could load some primitives here if wanted
        Model* triangle = new Triangle;
        addResource("triangle", triangle);
    }

    void ModelManager::cleanup()
    {
        unloadAll();
    }

    Model* ModelManager::loadResource(const char* filename)
    {
        return 0;
    }
}
