#include "../include/Map.h"

Map::Map()
{
    scene = 0;
    hFile = 0;
    terrainSize = 1025;
    terrainInitialized = false;
}

Map::~Map()
{
    if(terrainInitialized) //unset terrain
        scene->setTerrain(0, 0, 0, vector<Arya::Material*>(), 0, 0);
    terrainInitialized = false;

    if(hFile) Arya::FileSystem::shared().releaseFile(hFile);
    hFile = 0;
}

bool Map::initHeightData()
{
    if(hFile) return true;

    string heightMapString("heightmap.raw");
    heightMapString.insert(0, "textures/");
    hFile = Arya::FileSystem::shared().getFile(heightMapString);
    if(!hFile)
    {
        LOG_WARNING("Unable to load heightmap data!");
        return false;
    }
    scaleVector = vec3((float)terrainSize * 2.0f, 300.0f, (float)terrainSize * 2.0);
    return true;
}

bool Map::initGraphics(Scene* sc)
{
    if(terrainInitialized) return true;
    if(!hFile) return false;
    if(!sc) return false;
    scene = sc;

    terrainSize = 1025;
    vector<Arya::Material*> tileSet;
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("grass.tga"));
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("rock.tga"));
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("snow.tga"));
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("dirt.tga"));

    if(!scene->setTerrain(hFile->getData(), terrainSize, "watermap.raw", tileSet, Arya::TextureManager::shared().getTexture("clouds.jpg"), Arya::TextureManager::shared().getTexture("splatmap.tga")))
        return false;

    mat4 scaleMatrix = glm::scale(mat4(1.0), scaleVector);
    scene->getTerrain()->setScaleMatrix(scaleMatrix);

    terrainInitialized = true;
    return true;
}

float Map::heightAtGroundPosition(float x, float z)
{
    if(!hFile)
    {
        LOG_WARNING("Querying height, but no heightmap set");
        return 0.0;
    }

    if( x >= terrainSize || x <= -terrainSize || z >= terrainSize || z <= -terrainSize )
    {
        LOG_WARNING("Querying height outside of terrain!");
        return 0.0;
    }

    unsigned short h;
    unsigned short* heights = (unsigned short*)hFile->getData();

    int index = (int)((z + terrainSize)/2.0)*terrainSize + (int)((x + (terrainSize))/2.0);
    h = heights[index];
    return scaleVector.y*(h / 65535.0);
}

