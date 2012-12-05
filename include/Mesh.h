#pragma once

#include <GL/glew.h>

namespace Arya
{
    class Mesh
    {
        public:
            Mesh();
            ~Mesh();

            //GLuint getVAO() { return vaoHandle; }
            //GLuint getVertexBuffer() { return vertexBuffer; }
            //GLsizei getVertexCount() { return vertexCount; }
            //GLenum getPrimitiveType() { return primitiveType; }

            void addRef(){ refCount++; }
            void release(){ refCount--; }
            int getRefCount() const { return refCount; }

            GLuint vaoHandle;
            GLuint vertexBuffer;
            GLsizei vertexCount;
            GLenum primitiveType;

        private:
            int refCount;
    };
}
