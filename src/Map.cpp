#include "Map.h"
#include "Terrain.h"
#include "Textures.h"
#include "Camera.h"

#include "common/Logger.h"

#include <vector>
using std::vector;

namespace Arya
{
    Map::Map(const char* hm, vector<Texture*> ts, Texture* sm)
    {
        terrain = 0;
        setTerrain(hm, ts, sm);
    }

    Map::~Map()
    {
        if(terrain) delete terrain;
    }

    void Map::setTerrain(const char* hm, vector<Texture*> ts, Texture* sm)
    {
        if(!hm || !(ts.size()) || !sm) {
            if(terrain) delete terrain;
            terrain = 0;
            return;
        }

        Terrain* newTerrain = new Terrain(hm, ts, sm);
        if(!newTerrain->init()) {
            LOG_ERROR("Could not initialize terrain");
            delete newTerrain;
            return;
        }

        if(terrain) delete terrain;
        terrain = newTerrain;
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
