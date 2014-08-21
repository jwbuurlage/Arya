#include "World.h"
#include "EntitySystem.h"
#include "Terrain.h"

namespace Arya
{
    World::World()
    {
        entitySystem = new EntitySystem;
        terrain = new Terrain;
        //skybox = new Skybox;
    }

    World::~World()
    {
        //delete skybox;
        delete terrain;
        delete entitySystem;
    }

}
