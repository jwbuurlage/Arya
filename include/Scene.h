#pragma once

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "Root.h"

using std::string;
using std::vector;
using std::map;
using glm::vec3;

namespace Arya
{
    class Model;
    class Object;
    class Map;
    class Camera;
    class Shader;
    class ShaderProgram;

    class Scene : public FrameListener
    {
        public:
            Scene();
            ~Scene();

            bool isInitialized() const { return initialized; }

            void cleanup();
            void render();

            void onFrame(float elapsedTime);

            bool setMap(const char* hm, const char* wm, vector<Material*> ts, Texture* cm, Texture* sm);
            Map* getMap() const { return currentMap; };
            Camera* getCamera() { return camera; };

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
			vec3 lightDirection;

            Map* currentMap;
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
    };
}
