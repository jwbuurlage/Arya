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
            void setPosition(vec3 pos){ position = pos; updateMatrix = true; }
            const vec3& getPosition() const { return position; }

            const mat4& getMoveMatrix();

            //TODO: Add refcount
            //or even give 'Model' a list of Objects
            //so we can do optimized OpenGL Instancing
            void setModel(Model* m){ model = m; }
            Model* getModel() const { return model; }

        private:
            //Only Scene can make Objects
            friend class Scene;
            Object();
            ~Object();

            Model* model;

            vec3 position;
            float pitch;
            float yaw;

            mat4 mMatrix; //cached
            bool updateMatrix;
    };
}
