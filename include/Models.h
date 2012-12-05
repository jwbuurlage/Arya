#pragma once
#include "Mesh.h"
#include "Materials.h"
#include <vector>

using std::vector;

namespace Arya
{
    enum ModelType
    {
        ModelTypeUnkown = 0,
        ModelTypeStatic = 1,
        VertexAnimated = 2,
        BoneAnimated = 3
    };

    class Model
    {
        public:
            Model();
            ~Model();

            ModelType modelType;

            //Adds refcount to mesh as well
            void addMesh(Mesh* mesh);

            const vector<Mesh*>& getMeshes() const { return meshes; }

        private:
            vector<Mesh*> meshes;
            vector<Material*> materials;
    };
}
