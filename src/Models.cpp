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

    AnimationState* Model::createAnimationState()
    {
        //If bone, create BoneAnimationState
        //If vertex, create VertexAnimationState
        //else return 0
        return 0;
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

    bool ModelManager::initialize()
    {
        addResource("triangle", new Triangle);
        addResource("quad", new Quad);
        return true;
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
