#pragma once

#include <glm/glm.hpp>

using glm::vec3;
using glm::mat4;

namespace Arya
{
    class Mesh;
    class AnimationState;

    class Entity
    {
        private:
            //Only EntitySystem can create entities
            friend class EntitySystem;
            Entity();
            ~Entity();

        public:
            vec3 position;
            float pitch;
            float yaw;

            //Components
            Mesh* mesh;
            AnimationState* animationState;

            mat4 mMatrix; //cached
            bool updateMatrix;
    };
}
