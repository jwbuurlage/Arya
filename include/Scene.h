#pragma once

#include <string>
#include <vector>
#include <map>

#include "Root.h"

using std::string;
using std::vector;
using std::map;

namespace Arya
{
    class Object;
    class Terrain;
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

            Camera* getCamera() { return camera; };

        private:
            bool initialized;
            vector<Object*> objects;
            Terrain* terrain;
            Camera* camera;

            bool init();
            bool initShaders();

            Shader* basicVertex;
            Shader* basicFragment;
            ShaderProgram* basicProgram;
    };
}
