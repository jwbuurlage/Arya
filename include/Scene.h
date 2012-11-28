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

    class Scene
    {
        public:
            Scene();
            ~Scene();

            bool init();
            void cleanup();
            void render();
            Camera* getCamera() { return camera; };

        private:
            vector<Object*> objects;
            Terrain* terrain;
            Camera* camera;
    };
}
