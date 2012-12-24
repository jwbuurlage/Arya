#pragma once
#include "Arya.h"
#include <vector>

using std::vector;

using Arya::Scene;
using Arya::Terrain;
using Arya::Material;

class Map
{
    public:
        Map();
        ~Map();

        //loads map data: both server and client use this
        bool initHeightData();

        //loads graphical data: only for client
        bool initGraphics(Scene* scene);

        float heightAtGroundPosition(float x, float z);
    private:
        Scene* scene;
        Arya::File* hFile;
        int terrainSize;

        bool terrainInitialized;
};
