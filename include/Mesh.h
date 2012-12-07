#pragma once

#include <GL/glew.h>

namespace Arya
{
    class Mesh
    {
        public:
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

            //Only Model and ModelManger can create meshes
            friend class ModelManager;
            friend class Model;
            Mesh();
            ~Mesh();
    };
}
