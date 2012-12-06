#pragma once
#include "common/Singleton.h"
#include "Resources.h"
#include "Mesh.h"
#include "Materials.h"
#include <vector>
#include <GL/glew.h>

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

    class ModelManager : public Singleton<ModelManager>, public ResourceManager<Model> {
        public:
            ModelManager();
            ~ModelManager();

            int initialize();
            void cleanup();

            Model* getModel(const char* filename){ return getResource(filename); }
        private:
            Model* loadResource( const char* filename );
    };
}
