#include "Objects.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Arya
{
    Object::Object()
    {
        model = 0;
        updateMatrix = true;
        position = vec3(0.0, 0.0, 0.0);
        pitch = 0.0f;
        yaw = 0.0f;
    }

    Object::~Object()
    {
    }

    const mat4& Object::getMoveMatrix()
    {
        if(updateMatrix)
        {
            mMatrix = glm::translate(mat4(1.0f), position);
            mMatrix = glm::rotate(mMatrix, yaw, vec3(0.0, 1.0, 0.0));
            mMatrix = glm::rotate(mMatrix, pitch, vec3(1.0, 0.0, 0.0));
        }
        return mMatrix;
    }
}

