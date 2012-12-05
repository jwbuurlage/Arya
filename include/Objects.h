#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

using glm::vec3;
using glm::mat4;
using std::vector;

class Mesh;

namespace Arya
{
    class Object
    {
        public:
            Object();
            ~Object(){}

            GLuint getVAO() { return vaoHandle; }

            void setPosition(vec3 pos){ position = pos; updateMatrix = true; }
            const vec3& getPosition() const { return position; }

            const mat4& getMoveMatrix();

        protected:
            vector<Mesh*> meshes;
            GLuint vaoHandle;

            vec3 position;
            float pitch;
            float yaw;

            mat4 mMatrix; //cached
            bool updateMatrix;
    };

    typedef Object StaticObject;
}
