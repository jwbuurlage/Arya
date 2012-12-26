#include "../include/Map.h"

Map::Map()
{
    scene = 0;
    hFile = 0;
    heightMapSize = 1025;
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
    heightMapSize = 1025;
    scaleVector = vec3(4000.0f, 300.0f, 4000.0f);
    return true;
}

bool Map::initGraphics(Scene* sc)
{
    if(terrainInitialized) return true;
    if(!hFile) return false;
    if(!sc) return false;
    scene = sc;

    vector<Arya::Material*> tileSet;
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("grass.tga"));
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("rock.tga"));
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("snow.tga"));
    tileSet.push_back(Arya::MaterialManager::shared().getMaterial("dirt.tga"));

    if(!scene->setTerrain(hFile->getData(), heightMapSize, "watermap.raw", tileSet, Arya::TextureManager::shared().getTexture("clouds.jpg"), Arya::TextureManager::shared().getTexture("splatmap.tga")))
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

    x = (x/scaleVector.x) + 0.5;
    z = (z/scaleVector.z) + 0.5;

    //x,z are in [0,1]x[0,1] now

    //first convert to integers before doing the
    //comparisons so that we can not get floating
    //point rounding bugs
    int index1 = (int)(z*heightMapSize);
    int index2 = (int)(x*heightMapSize);

    if( index1 >= heightMapSize || index1 <= 0 || index2 >= heightMapSize || index2 <= 0 )
    {
        LOG_WARNING("Querying height outside of terrain!");
        return 0.0;
    }

    unsigned short* heights = (unsigned short*)hFile->getData();

    int index = index1*heightMapSize + index2;
    return scaleVector.y*(heights[index] / 65535.0);
}

