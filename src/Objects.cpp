#include "Objects.h"
#include "Models.h"
#include "common/Logger.h"
#include <glm/gtc/matrix_transform.hpp>

namespace Arya
{
    Object::Object()
    {
        model = 0;
        animState = 0;
        updateMatrix = true;
        position = vec3(0.0, 0.0, 0.0);
        pitch = 0.0f;
        yaw = 0.0f;
        tintColor = vec3(0.5);
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

    void Object::setModel(Model* newModel)
    {
        if( model ) model->release();
        if( animState ) delete animState;

        //Set new model and get a new animation state object
        //(subclass of AnimationState)
        model = newModel;
        if(model)
        {
            animState = model->createAnimationState();
            model->addRef();
        }
    }

    void Object::setAnimation(const char* name)
    {
        if( animState ) animState->setAnimation(name);
    }

    void Object::updateAnimation(float elapsedTime)
    {
        if( animState ) animState->updateAnimation(elapsedTime);
    }
}

