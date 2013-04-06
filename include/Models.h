#pragma once
#include "common/Singleton.h"
#include "Resources.h"
#include "Mesh.h"
#include "Materials.h"
#include <vector>
#include <GL/glew.h>

using std::vector;

#include <glm/glm.hpp>
using glm::vec3;

namespace Arya
{
    enum ModelType
    {
        //Note: these constants match
        //the constants that are used
        //in the AryaModel file format
        ModelTypeUnkown = 0,
        ModelTypeStatic = 1,
        VertexAnimated = 2,
        BoneAnimated = 3
    };

    //Base class for an animation state, stored in each object
    //Only contains the state, not the full animation data
    //Subclasses (in the cpp file) can be bone positions or keyframes
    //The actual keyframe data (like timings) are in the Model
    class AnimationState
    {
        public:
            AnimationState(){}
            virtual ~AnimationState(){}

            virtual void setAnimation(std::string name) = 0;
            virtual void updateAnimation(float elapsedTime) = 0;

            virtual int getCurFrame() = 0;
            virtual float getInterpolation() = 0;

            //For currently set animation:
            virtual float getAnimationTime() = 0;
            //Change speed of current animation
            virtual void setAnimationTime(float newTime) = 0;
    };

    //Base class for animation data
    //This is stored inside the model, not in the object
    class AnimationData
    {
        public:
            AnimationData(){}
            virtual ~AnimationData(){}
    };

    class Model
    {
        public:
            ModelType modelType;

            const vector<Mesh*>& getMeshes() const { return meshes; }
            const vector<Material*>& getMaterials() const { return materials; }

            const AnimationData* getAnimationData() const { return animationData; }

            //Called by Object
            AnimationState* createAnimationState();
            void addRef(){ refCount++; }
            void release(){ refCount--; }

			vec3 getBoundingBoxVertex(int vertexNumber);
        protected:
            //Private constructor because only
            //ModelManager is allowed to create these
            friend class ModelManager;
            friend class ResourceManager<Model>;
            Model();
            virtual ~Model();

            //Adds refcount as well
            void addMesh(Mesh* mesh);
            void addMaterial(Material* mat);

            Mesh* createAndAddMesh();

            vector<Mesh*> meshes;
            vector<Material*> materials;

            AnimationData* animationData;

			float minX; // Values needed to define
			float maxX; // bounding box for model.
			float minY;
			float maxY;
			float minZ;
			float maxZ;

            int refCount;
            //TODO: keep a list of objects that
            //use this model. This way we can
            //use OpenGL instancing
    };

    class ModelManager : public Singleton<ModelManager>, public ResourceManager<Model> {
        public:
            ModelManager();
            ~ModelManager();

            bool initialize();
            void cleanup();

            Model* getModel(std::string filename){ return getResource(filename); }
        private:
            Model* loadResource(std::string filename );
    };
}
