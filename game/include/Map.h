#pragma once
#include "Arya.h"
#include <vector>

using std::vector;

using Arya::Scene;
using Arya::Terrain;
using Arya::Material;

class MapInfo;

class Map
{
    public:
        Map(MapInfo* _info);
        ~Map();

        //loads map data: both server and client use this
        bool initHeightData();

        //loads graphical data: only for client
        bool initGraphics(Scene* scene);

        float heightAtGroundPosition(float x, float z);
        float getSize() const { return scaleVector.x; }

    private:
        Scene* scene;
        Arya::File* hFile;
        vec3 scaleVector;

        bool terrainInitialized;
		MapInfo* info;
};
