#include "../include/Map.h"
#include "../include/MapInfo.h"
#include "../include/common/GameLogger.h"

#include <boost/algorithm/string.hpp>

Map::Map(MapInfo* _info)
{
    scene = 0;
    hFile = 0;
    terrainInitialized = false;
	info = _info;
}

Map::~Map()
{
#ifndef SERVERONLY
    if(terrainInitialized) //unset terrain
        scene->setTerrain(0, 0, 0, vector<Arya::Material*>(), 0, 0);
#endif
    terrainInitialized = false;

    if(hFile) Arya::FileSystem::shared().releaseFile(hFile);
    hFile = 0;

	if(info)
		delete info;
}

bool Map::initHeightData()
{
    if(hFile) return true;

    string heightMapString(info->heightmap);
    heightMapString.insert(0, "textures/");
    hFile = Arya::FileSystem::shared().getFile(heightMapString);
    if(!hFile)
    {
        GAME_LOG_WARNING("Unable to load heightmap data!");
        return false;
    }
    scaleVector = vec3(info->width, 150.0f, info->height);
    return true;
}

bool Map::initGraphics(Scene* sc)
{
#ifndef SERVERONLY
    if(terrainInitialized) return true;
    if(!hFile) return false;
    if(!sc) return false;
    scene = sc;

    vector<Arya::Material*> tileSet;
	vector<string> strs;
	boost::split(strs, info->tileset, boost::is_any_of(","));
	for(int i = 0; i < strs.size(); ++i)
		tileSet.push_back(Arya::MaterialManager::shared().getMaterial(strs[i]));

    if(!scene->setTerrain(hFile->getData(), 
				info->heightmapSize, "watermap.raw", 
				tileSet, 
				Arya::TextureManager::shared().getTexture("clouds.jpg"), 
				Arya::TextureManager::shared().getTexture(info->splatmap)))
        return false;

    mat4 scaleMatrix = glm::scale(mat4(1.0), scaleVector);
    scene->getTerrain()->setScaleMatrix(scaleMatrix);

    terrainInitialized = true;
#endif
    return true;
}

float Map::heightAtGroundPosition(float x, float z)
{
    if(!hFile)
    {
        GAME_LOG_WARNING("Querying height, but no heightmap set");
        return 0.0;
    }

    x = (x/scaleVector.x) + 0.5;
    z = (z/scaleVector.z) + 0.5;

    //x,z are in [0,1]x[0,1] now

    //first convert to integers before doing the
    //comparisons so that we can not get floating
    //point rounding bugs
    int index1 = (int)(z*info->heightmapSize);
    int index2 = (int)(x*info->heightmapSize);

    if( index1 >= info->heightmapSize || index1 <= 0 || index2 >= info->heightmapSize || index2 <= 0 )
    {
        GAME_LOG_WARNING("Querying height outside of terrain!");
        return 0.0;
    }

    unsigned short* heights = (unsigned short*)hFile->getData();

    int index = index1*info->heightmapSize + index2;
    return scaleVector.y*(heights[index] / 65535.0);
}

