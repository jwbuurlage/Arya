#pragma once

#include <GL/glew.h>

namespace Arya
{
    class Mesh
    {
        public:
            Mesh();
            ~Mesh() { };

            GLuint getVertexBuffer() { return vertexBuffer; }
            GLuint getColorBuffer() { return colorBuffer; }
            GLsizei getVertexCount() { return vertexCount; }
            GLenum getPrimitiveType() { return primitiveType; }

        protected:
            GLuint vertexBuffer;
            GLuint colorBuffer;
            GLsizei vertexCount;
            GLenum primitiveType;

            bool hasColor;
    };
}
