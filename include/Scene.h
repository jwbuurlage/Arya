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

            bool init();
            bool initShaders();
            void cleanup();
            void render();
            Camera* getCamera() { return camera; };

        private:
            vector<Object*> objects;
            Terrain* terrain;
            Camera* camera;

            ShaderProgram* basicProgram;
    };
}
