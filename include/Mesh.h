#include <GL/glew.h>

namespace Arya
{
    class Mesh
    {
        public:
            Mesh();
            ~Mesh();

            GLfloat* getVertexData() { return vertexData; }
            GLuint getVertexDataSize() { return vertexDataSize; }

        private:
            GLfloat* vertexData;
            GLuint vertexDataSize;
    };
}
