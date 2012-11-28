#pragma once

#include <string>
#include <vector>
#include <map>

using std::string;
using std::vector;
using std::map;

class Object;
class Terrain;

namespace Arya
{
    class Scene
    {
        public:
            Scene();
            ~Scene() { };

            bool init();
            void render();
            Camera* getCamera() { return camera; };

        private:
            vector<Object*> objects;
            Terrain* terrain;
            Camera* camera;
    };
}
