#include "Models.h"

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
}
