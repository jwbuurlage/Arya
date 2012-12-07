#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

using glm::vec3;
using glm::mat4;
using std::vector;


namespace Arya
{
    class Model;
    class AnimationState;

    class Object
    {
        public:
            void setPosition(vec3 pos){ position = pos; updateMatrix = true; }
            const vec3& getPosition() const { return position; }

            const mat4& getMoveMatrix();

            //setModel also recreates a new AnimationState object
            void setModel(Model* model);
            Model* getModel() const { return model; }

            void setAnimation(const char* name);

            AnimationState* getAnimationState() const { return animState; }
            void updateAnimation(float elapsedTime);

        private:
            //Only Scene can make Objects
            friend class Scene;
            Object();
            ~Object();

            Model* model;
            AnimationState* animState;

            vec3 position;
            float pitch;
            float yaw;

            mat4 mMatrix; //cached
            bool updateMatrix;
    };
}
