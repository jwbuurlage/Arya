#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

using glm::vec3;
using glm::mat4;
using std::vector;


namespace Arya
{
    class Model;

    class Object
    {
        public:
            Object();
            ~Object();

            void setPosition(vec3 pos){ position = pos; updateMatrix = true; }
            const vec3& getPosition() const { return position; }

            const mat4& getMoveMatrix();

            Model* model;

        private:
            vec3 position;
            float pitch;
            float yaw;

            mat4 mMatrix; //cached
            bool updateMatrix;
    };
}
