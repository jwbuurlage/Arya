#pragma once

#include "material.h"
#include <vector>
#include <glm/glm.hpp>
using std::vector;

namespace Arya
{
    class Terrain;
    class Camera;
    class Texture;
    class Scene;

    class Map
    {
        public:
            Map();
            ~Map();

            bool init(const char* hm, const char* wm, vector<Material*> ts, Texture* cm, Texture* sm);

            bool setTerrain(const char* hm, const char* wm, vector<Material*> ts, Texture* cm, Texture* sm);
            void render(Camera* cam);
            void update(float elapsedTime, Scene* scene);

            Terrain* getTerrain() const { return terrain; };

        private:
            Terrain* terrain;
    };
}
