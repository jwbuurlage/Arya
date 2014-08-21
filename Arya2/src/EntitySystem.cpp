#include "EntitySystem.h"
#include "Entity.h"

namespace Arya
{
    EntitySystem::EntitySystem()
    {
    }

    EntitySystem::~EntitySystem()
    {
        for(auto ent : entities)
            delete ent;
        entities.clear();
    }

    Entity* EntitySystem::createEntity()
    {
        Entity* ent = new Entity;
        entities.push_back(ent);
        return ent;
    }
}
