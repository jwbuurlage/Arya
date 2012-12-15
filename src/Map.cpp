#include "Map.h"
#include "Terrain.h"
#include "Textures.h"
#include "Camera.h"

#include "common/Logger.h"

#include <vector>
#include <glm/glm.hpp>
using std::vector;

namespace Arya
{
    Map::Map()
    {
        terrain = 0;
    }

    Map::~Map()
    {
        if(terrain) delete terrain;
    }

    bool Map::init(const char* hm, const char* wm, vector<Texture*> ts, Texture* cm, Texture* sm)
    {
        if(!setTerrain(hm, wm, ts, cm, sm)) {
            LOG_ERROR("Could not initialize map");
            return false;
        }

        return true;
    }

    bool Map::setTerrain(const char* hm, const char* wm, vector<Texture*> ts, Texture* cm, Texture* sm)
    {
        if(!hm || !wm || !(ts.size()) || !sm) {
            if(terrain) delete terrain;
            terrain = 0;
            return false;
        }

        Terrain* newTerrain = new Terrain(hm, wm, ts, cm, sm);
        if(!newTerrain->init()) {
            LOG_ERROR("Could not initialize terrain");
            delete newTerrain;
            return false;
        }

        if(terrain) delete terrain;
        terrain = newTerrain;

        return true;
    }

    void Map::render(Camera* camera)
    {
        if(terrain)
            terrain->render(camera);
    }

    void Map::update(float elapsedTime, Scene* scene)
    {
        if(terrain)
            terrain->update(elapsedTime, scene);
    }
}
