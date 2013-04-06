#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "Materials.h"
#include "Root.h"

using std::string;
using std::vector;
using std::map;
using glm::vec3;

namespace Arya
{
    class Model;
    class Object;
    class Terrain;
    class Camera;
    class Shader;
    class ShaderProgram;
	class FogMap;
	class MiniMap;

    class Scene : public FrameListener
    {
        public:
            Scene();
            virtual ~Scene();

            bool isInitialized() const { return initialized; }

            void cleanup();
            void render();

            void onFrame(float elapsedTime);

            bool setTerrain(char* heightData, int terrainSize, const char* waterMap, const vector<Material*>& tileSet, Texture* cloudMap, Texture* splatMap);
            Terrain* getTerrain() const { return currentTerrain; };

            Camera* getCamera() { return camera; };
            FogMap* getFogMap() { return fm; };
            MiniMap* getMiniMap() { return minimap; };

            Object* createObject();

            mat4 getLightOrthoMatrix() const { return lightOrthoMatrix; }
            GLuint getShadowDepthTextureHandle() const { return shadowDepthTextureHandle; }

        private:
            bool initialized;
			
            //TODO: think about whether we
            //want to make this into a linked list
            //since we will be removing objects
            //a lot (units dying)
            //Not a std::list though because this adds
            //overhead. We can do the trick where
            //we subclass Object in a clever way
            //so no extra space is needed.
            vector<Object*> objects;

            //lightDirection points TO the light
            //it should always be normalized
			vec3 lightDirection;

            Terrain* currentTerrain;
            Camera* camera;

            bool init();
            bool initShaders();

            // Shadows
            bool initShadowSupport();
            GLuint shadowFBOHandle;
            GLuint shadowDepthTextureHandle;
            mat4 orthoShadowCubeMatrix;
            mat4 rotateToLightDirMatrix;
            mat4 lightOrthoMatrix;

            ShaderProgram* basicProgram;
			FogMap* fm;
			MiniMap* minimap;
    };
}
