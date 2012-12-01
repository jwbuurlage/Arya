#pragma once

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

namespace Arya
{
    class Object;
    class Terrain;
    class Camera;
    class ShaderProgram;

    class Scene
    {
        public:
            Scene();
            ~Scene();

            bool isInitialized() const { return initialized; }

            void cleanup();
            void render();
            Camera* getCamera() { return camera; };

        private:
            bool initialized;
            vector<Object*> objects;
            Terrain* terrain;
            Camera* camera;

            bool init();
            bool initShaders();

            ShaderProgram* basicProgram;
    };
}
