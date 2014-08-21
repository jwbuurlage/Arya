#pragma once

namespace Arya
{
    class EntitySystem;
    class Terrain;
    class Skybox;

    class World
    {
        public:
            World();
            ~World();

            EntitySystem* getEntitySystem() const { return entitySystem; }
            Terrain*      getTerrain() const { return terrain; }
            Skybox*       getSkybox() const { return skybox; }

        private:
            EntitySystem*   entitySystem;
            Terrain*        terrain;
            Skybox*         skybox;

    };
}
