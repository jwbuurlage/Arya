#include <GL/glew.h>
#include <vector>

using std::vector;

class Mesh;

namespace Arya
{
    class Object
    {
        public:
            Object() { };
            ~Object() { };

            GLuint getVAO() { return vaoHandle; }

        protected:
            vector<Mesh*> meshes;
            GLuint vaoHandle;
    };

    class StaticObject : public Object
    {
        public:
            StaticObject();
            ~StaticObject();
    };
}
