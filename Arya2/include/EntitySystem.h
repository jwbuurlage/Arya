#pragma once

#include <list>
using std::list;

namespace Arya
{
    class Entity;

    class EntitySystem
    {
        public:
            EntitySystem();
            ~EntitySystem();

            Entity* createEntity();

        private:

            list<Entity*> entities;
    };
}
